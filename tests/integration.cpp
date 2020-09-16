#include <catch.hpp>
#include <util.h>

#include <one/agent/agent.h>
#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/health.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/arcus/server.h>
#include <one/game/game.h>

#include <chrono>

using namespace std::chrono;
using namespace game;
using namespace i3d::one;

// Todo: disabled. This test doesn't appear to test connection failure or add value.
TEST_CASE("Agent connection failure", "[.][integration]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19001;

    Game game(port);
    REQUIRE(game.init(1, 16, "name", "map", "mode", "version"));
    REQUIRE(game.one_server_wrapper().status() ==
            OneServerWrapper::Status::waiting_for_client);

    Agent agent;
    REQUIRE(agent.init(address, port) == 0);

    game.update();
    REQUIRE(agent.update() == ONE_ERROR_NONE);
    REQUIRE(agent.client().status() == Client::Status::handshake);
}

TEST_CASE("long:Handshake timeout", "[integration]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19003;

    Game game(port);
    REQUIRE(game.init(1, 16, "test game", "test map", "test mode", "test version"));

    Agent agent;
    REQUIRE(agent.init(address, port) == 0);
    // Update agent and game one time to initiate the connection.
    REQUIRE(agent.update() == int(ONE_ERROR_NONE));
    game.update();
    REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::handshake);

    // Update the game only, so that the agent doesn't progress the handshake,
    // until the handshake timeout expires.
    int ms = Connection::handshake_timeout_seconds * 1000;
    for_sleep(5, ms / 4, [&]() {
        game.update();
        if (game.one_server_wrapper().status() ==
            OneServerWrapper::Status::waiting_for_client)
            return true;
        return false;
    });
    REQUIRE(game.one_server_wrapper().status() ==
            OneServerWrapper::Status::waiting_for_client);

    // Shut down the game and restart it, the client should reconnect automatically.
    game.shutdown();
}

TEST_CASE("long:Reconnection", "[integration]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19003;

    Game game(port);
    REQUIRE(game.init(1, 16, "test game", "test map", "test mode", "test version"));

    Agent agent;
    REQUIRE(agent.init(address, port) == 0);

    pump_updates(10, 1, agent, game);
    REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::ready);
    REQUIRE(agent.client().status() == Client::Status::ready);

    // Shut down the game and restart it, the client should reconnect automatically.
    game.shutdown();

    // Sleep for long enough such that the next update should result in the agent
    // realizing that the game server is no longer there and the agent should
    // enter a state of reconnecting to the server.
    sleep(HealthChecker::health_check_receive_interval_seconds * 1000);
    REQUIRE(agent.update() == ONE_ERROR_CONNECTION_HEALTH_TIMEOUT);
    REQUIRE(agent.client().status() == Client::Status::connecting);

    // Restart the server and it should be waiting for client.
    REQUIRE(game.init(1, 16, "test game", "test map", "test mode", "test version"));
    REQUIRE(game.one_server_wrapper().status() ==
            OneServerWrapper::Status::waiting_for_client);

    // Update both and the agent should reconnect.
    pump_updates(10, 1, agent, game);
    REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::ready);
    REQUIRE(agent.client().status() == Client::Status::ready);
}

TEST_CASE("long:Maintain connection", "[integration]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19003;

    Game game(port);
    REQUIRE(game.init(1, 16, "test game", "test map", "test mode", "test version"));

    Agent agent;
    REQUIRE(agent.init(address, port) == 0);

    const auto start = steady_clock::now();

    // Service both game and agent updates for a longer period of time (2x
    // health checks +1 second) so that keeping the connections alive requires
    // health messages.
    while (duration_cast<seconds>(steady_clock::now() - start).count() <
           HealthChecker::health_check_receive_interval_seconds * 2 + 1) {
        pump_updates(10, 1, agent, game);
        REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::ready);
        REQUIRE(agent.client().status() == Client::Status::ready);
        sleep(100);
    }
    // Both should still be connected.
    REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::ready);
    REQUIRE(agent.client().status() == Client::Status::ready);
}
