#include <catch.hpp>
#include <util.h>
#include <one/agent/agent.h>
#include <one/game/game.h>

#include <iostream>

using namespace one;

void live_state_callback(int player, int max_player, const std::string &name,
                         const std::string &map, const std::string &mode,
                         const std::string &version) {
    std::cout << "send_live_callback:"
              << " player " << player << " max_player : " << max_player << " name: " << name
              << " map: " << map << " mode: " << mode << " version: " << version << std::endl;
}

TEST_CASE("Agent connects to a game", "[agent]") {
    Agent agent;
    REQUIRE(agent.connect("127.0.0.1", 9001) == 0);
    REQUIRE(agent.status() == 0);

    Game game(9001, 1, 16, "test", "test", "test", "test");
    REQUIRE(game.init() == 0);

    // Need to add handshake logic when ready.
}

TEST_CASE("Agent failed connects to a game", "[agent]") {
    Agent agent;
    REQUIRE(agent.connect("127.0.0.1", 9001) == 0);
    REQUIRE(agent.status() == 0);

    Game game(10001, 1, 16, "test", "test", "test", "test");
    REQUIRE(game.init() == 0);

    // Need to add handshake logic when ready & check for failure.
}

TEST_CASE("Agent send request to game", "[agent]") {
    Game game(9001, 10, 54, "test game", "test map", "test mode", "test version");
    REQUIRE(game.init() == 0);
    REQUIRE(game.set_live_state_request_callback() == 0);

    Agent agent;
    REQUIRE(agent.set_live_state_callback(live_state_callback) == 0);
    REQUIRE(agent.connect("127.0.0.1", 9001) == 0);
    REQUIRE(agent.status() == 0);

    // First agent request
    REQUIRE(agent.send_live_state_request() == 0);
    REQUIRE(agent.update() == 0);

    sleep(1);

    REQUIRE(game.tick() == -1);  // FIXME: when the socket & handshake if fully merged in.
    REQUIRE(agent.update() == 0);

    // TODO: add more agent request & custom messages.

    // Last agent request
    REQUIRE(agent.send_soft_stop() == 0);
    REQUIRE(agent.update() == 0);

    sleep(1);

    REQUIRE(game.tick() == -1);  // FIXME: when the socket & handshake if fully merged in.
    REQUIRE(agent.update() == 0);

    REQUIRE(game.shutdown() == 0);
}
