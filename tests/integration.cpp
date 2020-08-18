#include <catch.hpp>
#include <util.h>

#include <one/agent/agent.h>
#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/arcus/server.h>
#include <one/game/game.h>

#include <iostream>

using namespace one;

bool error_callback_has_been_called = false;
bool live_state_callback_has_been_called = false;
bool allocated_callback_has_been_called = false;
bool meta_data_callback_has_been_called = false;
bool host_information_callback_has_been_called = false;
bool soft_stop_callback_has_been_called = false;

void error_callback(void *) {
    std::cout << "error_callback!" << std::endl;
    error_callback_has_been_called = true;
}

void live_state_callback(void *, int player, int max_player, const std::string &name,
                         const std::string &map, const std::string &mode,
                         const std::string &version) {
    std::cout << "send_live_callback:"
              << " player " << player << " max_player : " << max_player << " name: " << name
              << " map: " << map << " mode: " << mode << " version: " << version << std::endl;
    live_state_callback_has_been_called = true;
}

void allocated_callback(void *, void *) {
    std::cout << "allocated_callback!" << std::endl;
    allocated_callback_has_been_called = true;
}

void meta_data_callback(void *, void *) {
    std::cout << "meta_data_callback!" << std::endl;
    allocated_callback_has_been_called = true;
}

void host_information_callback(void *) {
    std::cout << "host_information_callback!" << std::endl;
    host_information_callback_has_been_called = true;
}

void soft_stop_callback(void *, int timeout) {
    std::cout << "soft_stop_callback:"
              << " timeout " << timeout << std::endl;
    soft_stop_callback_has_been_called = true;
}

void handshake(Agent &agent, Game &game) {
    for_sleep(10, 10, [&]() {
        REQUIRE(game.update() == 0);

        REQUIRE(agent.update() == 0);

        if (game.status() == static_cast<int>(Server::Status::ready) &&
            agent.status() == static_cast<int>(Client::Status::ready))
            return true;

        return false;
    });
    auto ready = game.status() == static_cast<int>(Server::Status::ready) &&
                 agent.status() == static_cast<int>(Client::Status::ready);
    REQUIRE(ready);
}

TEST_CASE("Agent connects to a game & send requests", "[agent]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 9001;

    Game game(port, 1, 1, 16, "test", "test", "test", "test");
    REQUIRE(game.init(1024, 1024) == 0);
    REQUIRE(game.set_live_state_request_callback() == 0);
    REQUIRE(game.set_allocated_request_callback(allocated_callback, nullptr) == 0);
    REQUIRE(game.status() == static_cast<int>(Server::Status::listening));

    Agent agent;
    REQUIRE(agent.connect(address, port, 1024, 1024) == 0);
    REQUIRE(agent.status() == static_cast<Error>(Client::Status::handshake));
    REQUIRE(agent.set_error_response_callback(error_callback, nullptr) == 0);
    REQUIRE(agent.set_live_state_response_callback(live_state_callback, nullptr) == 0);
    REQUIRE(agent.set_host_information_request_callback(host_information_callback, nullptr) == 0);

    handshake(agent, game);

    // FIXME: remove one the messages are sent over the socket.
    REQUIRE(game.shutdown() == 0);
    return;

    {
        REQUIRE(agent.send_soft_stop_request(1000) == 0);
        for_sleep(5, 1, [&]() {
            REQUIRE(agent.update() == 0);
            REQUIRE(game.update() == 0);
            return soft_stop_callback_has_been_called;
        });
        REQUIRE(soft_stop_callback_has_been_called == true);
    }

    // error_response
    {
        REQUIRE(game.send_error_response() == 0);
        REQUIRE(agent.update() == 0);
        sleep(10);
        REQUIRE(game.update() == 0);
        REQUIRE(error_callback_has_been_called == true);
    }

    // live_state_request
    {
        REQUIRE(agent.send_live_state_request() == 0);
        REQUIRE(agent.update() == 0);
        sleep(10);
        REQUIRE(game.update() == 0);
        REQUIRE(live_state_callback_has_been_called == true);
    }

    // allocated_request
    {
        Array array;
        REQUIRE(agent.send_allocated_request(&array) == 0);
        REQUIRE(agent.update() == 0);
        sleep(10);
        REQUIRE(game.update() == 0);
        REQUIRE(allocated_callback_has_been_called == true);
    }

    // meta_data_request
    {
        Array array;
        REQUIRE(agent.send_meta_data_request(&array) == 0);
        REQUIRE(agent.update() == 0);
        sleep(10);
        REQUIRE(game.update() == 0);
        REQUIRE(meta_data_callback_has_been_called == true);
    }

    // host_information_request
    {
        REQUIRE(game.send_host_information_request() == 0);
        REQUIRE(game.update() == 0);
        sleep(10);
        REQUIRE(agent.update() == 0);
        REQUIRE(host_information_callback_has_been_called == true);
    }

    // TODO: add more agent request & custom messages.

    REQUIRE(game.shutdown() == 0);
}
