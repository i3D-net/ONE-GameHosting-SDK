#include <catch.hpp>
#include <util.h>

#include <one/agent/agent.h>
#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/object.h>
#include <one/game/game.h>
#include <one/game/log.h>

using namespace one_integration;
using namespace i3d::one;

TEST_CASE("life cycle", "[fake game]") {
    Game game;
    REQUIRE(game.init(19001, 54, "test game", "test map", "test mode", "test version"));
    game.shutdown();
    REQUIRE(allocation::alloc_count() > 0);
    REQUIRE(allocation::free_count() == allocation::alloc_count());
}

TEST_CASE("connection error handling", "[fake game]") {
    Game game;
    REQUIRE(game.init(19001, 54, "test game", "test map", "test mode", "test version"));

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
    REQUIRE(game.init(port, 16, "name", "map", "mode", "version"));
    REQUIRE(game.one_server_wrapper().status() ==
            OneServerWrapper::Status::waiting_for_client);

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

        bool passed = wait_until(200, [&]() {
            agent.update();
            game.update();
            return game.soft_stop_receive_count() == 1;
        });
    }

    // allocated.
    {
        Object map;
        auto err = map.set_val_string("key", "map");
        REQUIRE(!is_error(err));
        err = map.set_val_string("value", "islands_large");
        REQUIRE(!is_error(err));

        Object max_players;
        err = max_players.set_val_string("key", "maxPlayers");
        REQUIRE(!is_error(err));
        err = max_players.set_val_string("value", "16");
        REQUIRE(!is_error(err));

        Array data;
        data.push_back_object(map);
        data.push_back_object(max_players);

        REQUIRE(agent.send_allocated(data) == 0);

        bool passed = wait_until(200, [&]() {
            agent.update();
            game.update();
            return game.allocated_receive_count() == 1 &&
                   agent.application_instance_status_receive_count() == 1;
        });
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

        bool passed = wait_until(200, [&]() {
            agent.update();
            game.update();
            return game.allocated_receive_count() == 1;
        });
    }

    //--------------------------------------------------------------------------
    // Sending from game to agent.

    // live state.
    auto &wrapper = game.one_server_wrapper();
    {
        // Agent should already receive one game state update when the connection
        // is established.
        REQUIRE(agent.live_state_receive_count() == 1);
        game.set_player_count(game.player_count() + 1);
        bool passed = wait_until(200, [&]() {
            game.update();
            agent.update();
            return agent.live_state_receive_count() == 2;
        });
        REQUIRE(passed);
    }

    game.shutdown();
}
