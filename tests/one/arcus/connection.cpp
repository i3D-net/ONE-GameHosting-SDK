#include <catch.hpp>
#include <tests/one/arcus/util.h>

#include <iostream>
#include <map>
#include <thread>

#include <one/fake/arcus/agent/agent.h>
#include <one/arcus/c_error.h>
#include <one/fake/arcus/game/log.h>
#include <one/fake/arcus/game/game.h>
#include <one/fake/arcus/game/one_server_wrapper.h>

using namespace one_integration;
using namespace i3d::one;

void pump_updates_with_logs(int count, int ms_per_loop, Agent &agent,
                            one_integration::Game &game) {
    for_sleep(count, ms_per_loop, [&]() {
        game.update();
        auto err = agent.update();
        if (one_is_error(err)) {
            L_ERROR(one_error_text(err));
        }
        if (agent.client().status() == Client::Status::ready &&
            game.one_server_wrapper().status() ==
                one_integration::OneServerWrapper::Status::ready)
            return true;
        return false;
    });
}

void pump_updates_with_logs(int count, int ms_per_loop, one_integration::Game &game) {
    for_sleep(count, ms_per_loop, [&]() {
        game.update();
        return true;
    });
}

void pump_updates_with_logs(int count, int ms_per_loop, Agent &agent) {
    for_sleep(count, ms_per_loop, [&]() {
        agent.update();
        return true;
    });
}

TEST_CASE("connect:single game connecting to multiple agents", "[concurrency]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19112;

    Game game;
    game.init(port, 16, "name", "map", "mode", "version", seconds(0));
    game.set_quiet(false);

    for (int i = 0; i < 100; ++i) {
        Agent *agent = new Agent();
        REQUIRE(agent != nullptr);
        agent->init(address, port);
        agent->set_quiet(false);

        pump_updates_with_logs(10, 1, *agent, game);

        // This will call agent's client shutdown function.
        delete agent;

        pump_updates_with_logs(10, 1, game);
    }

    game.shutdown();
}

TEST_CASE("connect:single agent connecting to multiple games", "[concurrency]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19112;

    Agent agent;
    agent.init(address, port);
    agent.set_quiet(false);

    for (int i = 0; i < 100; ++i) {

        Game *game = new Game();
        REQUIRE(game != nullptr);
        game->init(port, 16, "name", "map", "mode", "version", seconds(0));
        game->set_quiet(false);

        pump_updates_with_logs(10, 1, agent, *game);
        game->shutdown();
        delete game;
        pump_updates_with_logs(10, 1, agent);
    }
}

TEST_CASE("connect:multiple agents connecting to multiple games", "[concurrency]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19112;

    // Game shutdown first
    for (int i = 0; i < 100; ++i) {
        Agent *agent = new Agent();
        REQUIRE(agent != nullptr);
        agent->init(address, port);
        agent->set_quiet(false);

        Game *game = new Game();
        REQUIRE(game != nullptr);
        game->init(port, 16, "name", "map", "mode", "version", seconds(0));
        game->set_quiet(false);

        pump_updates_with_logs(10, 1, *agent, *game);
        game->shutdown();
        delete game;
        pump_updates_with_logs(10, 1, *agent);
        // This will call agent's client shutdown function.
        delete agent;
    }

    // Agent shutdown first
    for (int i = 0; i < 100; ++i) {
        Agent *agent = new Agent();
        REQUIRE(agent != nullptr);
        agent->init(address, port);
        agent->set_quiet(false);

        Game *game = new Game();
        REQUIRE(game != nullptr);
        game->init(port, 16, "name", "map", "mode", "version", seconds(0));
        game->set_quiet(false);

        pump_updates_with_logs(10, 1, *agent, *game);
        // This will call agent's client shutdown function.
        delete agent;
        pump_updates_with_logs(10, 1, *game);
        game->shutdown();
        delete game;
    }
}
