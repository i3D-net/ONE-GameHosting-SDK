#include <catch.hpp>
#include <util.h>

#include <one/game/game.h>

using namespace one;

TEST_CASE("fake game life cycle", "[fake game]") {
    Game fakeGame(4242);
    REQUIRE(fakeGame.init() == 0);
    REQUIRE(fakeGame.shutdown() == 0);
}

TEST_CASE("fake game listen, status, update & close", "[fake game]") {
    Game fakeGame(4242);
    REQUIRE(fakeGame.init() == 0);
    REQUIRE(fakeGame.tick() == -1);  // FIXME: when the socket & handshake if fully merged in.
    sleep(1);
    REQUIRE(fakeGame.tick() == -1);  // FIXME: when the socket & handshake if fully merged in.
    sleep(1);
    REQUIRE(fakeGame.tick() == -1);  // FIXME: when the socket & handshake if fully merged in.
    REQUIRE(fakeGame.shutdown() == 0);
}
