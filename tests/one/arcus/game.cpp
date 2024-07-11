#include <catch.hpp>
#include <tests/one/arcus/util.h>

#include <one/fake/arcus/agent/agent.h>
#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/object.h>
#include <one/fake/arcus/game/game.h>
#include <one/fake/arcus/game/log.h>

using namespace one_integration;
using namespace i3d::one;

TEST_CASE("life cycle", "[fake game]") {
    // Some allocations are global, e.g. the error static string lookup, and
    // will not be freed during shutdown.
    // Comparing before and after to avoid previous tests allocation pollution.
    auto alloc_before = allocation::alloc_count();
    auto free_before = allocation::free_count();
    Game game;
    REQUIRE(game.init(19001, 54, "test game", "test map", "test mode", "test version", std::chrono::seconds(0)));
    game.shutdown();
    REQUIRE(allocation::alloc_count() > 0);
    auto alloc_after = allocation::alloc_count();
    auto free_after = allocation::free_count();
    REQUIRE((alloc_after - alloc_before) >= (free_after - free_before));
    int titi = 0;
}

TEST_CASE("connection error handling", "[fake game]") {
    Game game;
    REQUIRE(game.init(19001, 54, "test game", "test map", "test mode", "test version", std::chrono::seconds(0)));

    // Connect a client socket to fake a scriptable agent.
    Socket client;
    client.init();
    REQUIRE(!is_error(client.connect("127.0.0.1", 19001)));

    // Update game so that connection starts and handhsake is sent.
    auto pump_game = [&]() {
        for_sleep(5, 1, [&]() {
            game.update();
            return false;
        });
    };
    pump_game();

    REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::handshake);

    // Receive hello from server.
    codec::Hello hello = {0};
    size_t received = 0;
    auto result = client.receive(&hello, codec::hello_size(), received);
    REQUIRE(!is_error(result));
    REQUIRE(received == codec::hello_size());
    REQUIRE(codec::validate_hello(hello));

    // Send wrong opcode back.
    static codec::Header hello_header = {0};
    hello_header.opcode = static_cast<char>(Opcode::soft_stop);
    size_t sent = 0;
    result = client.send(&hello_header, codec::header_size(), sent);
    REQUIRE(!is_error(result));
    REQUIRE(sent == codec::header_size());

    // Update the game, at some point it should error while trying to process
    // the handshake.
    for_sleep(5, 1, [&]() {
        game.update();
        return false;
    });

    pump_game();
    REQUIRE(game.one_server_wrapper().status() ==
            OneServerWrapper::Status::waiting_for_client);

    game.shutdown();
}

TEST_CASE("agent and game messaging", "[fake game]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19002;

    Game game;
    REQUIRE(game.init(port, 16, "name", "map", "mode", "version", std::chrono::seconds(0)));
    REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::waiting_for_client);

    Agent agent;
    REQUIRE(agent.init(address, port) == 0);
    REQUIRE(agent.update() == ONE_ERROR_NONE);
    REQUIRE(agent.client().status() == Client::Status::handshake);

    pump_updates(10, 1, agent, game);
    REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::ready);
    REQUIRE(agent.client().status() == Client::Status::ready);

    // soft stop.
    {
        REQUIRE(agent.send_soft_stop(1000) == 0);

        pump_updates(10, 1, agent, game);
        bool passed = wait_until(200, [&]() {
            agent.update();
            game.update();
            return game.soft_stop_receive_count() == 1;
        });
        REQUIRE(passed);
    }

    // allocated.
    {
        Object players;
        auto err = players.set_val_string("key", "players");
        REQUIRE(!is_error(err));
        err = players.set_val_string("value", "10");
        REQUIRE(!is_error(err));

        Object duration;
        err = duration.set_val_string("key", "duration");
        REQUIRE(!is_error(err));
        err = duration.set_val_string("value", "16");
        REQUIRE(!is_error(err));

        Array data;
        data.push_back_object(players);
        data.push_back_object(duration);

        REQUIRE(agent.send_allocated(data) == 0);

        pump_updates(10, 1, agent, game);
        bool passed = wait_until(200, [&]() {
            agent.update();
            game.update();
            return game.allocated_receive_count() == 1;
        });
        REQUIRE(passed);
    }

    // metadata.
    {
        Object map;
        auto err = map.set_val_string("key", "map");
        REQUIRE(!is_error(err));
        err = map.set_val_string("value", "islands_large");
        REQUIRE(!is_error(err));

        Object mode;
        err = mode.set_val_string("key", "mode");
        REQUIRE(!is_error(err));
        err = mode.set_val_string("value", "BR");
        REQUIRE(!is_error(err));

        Object type;
        err = type.set_val_string("key", "type");
        REQUIRE(!is_error(err));
        err = type.set_val_string("value", "squads");
        REQUIRE(!is_error(err));

        Array data;
        data.push_back_object(map);
        data.push_back_object(mode);
        data.push_back_object(type);

        Array array;
        REQUIRE(agent.send_metadata(data) == 0);

        pump_updates(10, 1, agent, game);
        bool passed = wait_until(200, [&]() {
            agent.update();
            game.update();
            return game.metadata_receive_count() == 1;
        });
        REQUIRE(passed);
    }

    // reverse metadata.
    {
        game.send_reverse_metadata("islands_large", "BR", "squads");

        pump_updates(10, 1, agent, game);
        bool passed = wait_until(200, [&]() {
            agent.update();
            game.update();
            return agent.reverse_meta_data_receive_count() == 1;
        });
        REQUIRE(passed);
    }

    // custom command.
    {
        Object command;
        auto err = command.set_val_string("key", "command");
        REQUIRE(!is_error(err));
        err = command.set_val_string("value", "custom command");
        REQUIRE(!is_error(err));

        Object argument;
        err = argument.set_val_string("key", "argument");
        REQUIRE(!is_error(err));
        err = argument.set_val_string("value", "argument 1");
        REQUIRE(!is_error(err));

        Array data;
        data.push_back_object(command);
        data.push_back_object(argument);

        REQUIRE(agent.send_custom_command(data) == 0);

        pump_updates(10, 1, agent, game);
        bool passed = wait_until(200, [&]() {
            agent.update();
            game.update();
            return game.custom_command_receive_count() == 1;
        });
        REQUIRE(passed);
    }

    //--------------------------------------------------------------------------
    // Sending from game to agent.

    // live state.
    auto &wrapper = game.one_server_wrapper();
    {
        // Agent should already receive game state update when the connection
        // is established.
        REQUIRE(agent.live_state_receive_count() == 1);
        game.set_player_count(game.player_count() + 1);
        pump_updates(10, 1, agent, game);
        bool passed = wait_until(200, [&]() {
            game.update();
            agent.update();
            return agent.live_state_receive_count() == 2;
        });
        REQUIRE(passed);
    }

    game.shutdown();
}
