#include <catch.hpp>
#include <util.h>

#include <one/arcus/error.h>
#include <one/game/game.h>

using namespace one;

TEST_CASE("fake game life cycle", "[fake game]") {
    Game fakeGame(9001, 10, 54, "test game", "test map", "test mode", "test version");
    REQUIRE(fakeGame.init() == 0);
    REQUIRE(fakeGame.shutdown() == 0);
}

TEST_CASE("fake game listen, status, update & close", "[fake game]") {
    Game fakeGame(9001, 10, 54, "test game", "test map", "test mode", "test version");
    REQUIRE(fakeGame.init() == 0);
    REQUIRE(is_error(fakeGame.tick()));  // FIXME: when the socket & handshake if fully merged in.
    sleep(1);
    REQUIRE(is_error(fakeGame.tick()));  // FIXME: when the socket & handshake if fully merged in.
    sleep(1);
    REQUIRE(is_error(fakeGame.tick()));  // FIXME: when the socket & handshake if fully merged in.
    REQUIRE(fakeGame.shutdown() == 0);
}
