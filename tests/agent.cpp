#include <catch.hpp>
#include <util.h>
#include <one/agent/agent.h>
#include <one/game/game.h>

using namespace one;

TEST_CASE("Agent standalone tests", "[agent]") {
    Agent agent = Agent();
    REQUIRE(agent.connect("127.0.0.1", 9001) == 0);
    REQUIRE(agent.status() == 0);
}

TEST_CASE("Agent send request to game", "[agent]") {
    Game game(9001);
    REQUIRE(game.init() == 0);

    Agent agent = Agent();
    REQUIRE(agent.connect("127.0.0.1", 9001) == 0);
    REQUIRE(agent.status() == 0);

    // First agent request
    REQUIRE(agent.request_server_info() == 0);

    sleep(1);

    REQUIRE(game.tick() == -1);  // FIXME: when the socket & handshake if fully merged in.
    REQUIRE(agent.update() == 0);

    // TODO: add more agent request & custom messages.

    // Last agent request
    REQUIRE(agent.send_soft_stop() == 0);

    sleep(1);

    REQUIRE(game.tick() == -1);  // FIXME: when the socket & handshake if fully merged in.
    REQUIRE(agent.update() == 0);

    REQUIRE(game.deinit() == 0);
}
