#include <catch.hpp>
#include <util.h>

#include <one/game/game.h>

using namespace one;

TEST_CASE("fake game life cycle", "[fake game]") {
    Game fakeGame(4242);
    REQUIRE(fakeGame.init() == -1);    // TODO: toggle it to == 0, when the implementation is done.
    REQUIRE(fakeGame.deinit() == -1);  // TODO: toggle it to == 0, when the implementation is done.
}

TEST_CASE("fake game listen, status, update & close", "[fake game]") {
    Game fakeGame(4242);
    REQUIRE(fakeGame.init() == -1);  // TODO: toggle it to == 0, when the implementation is done.
    REQUIRE(fakeGame.tick() == -1);  // TODO: toggle it to == 0, when the implementation is done.
    sleep(1);
    REQUIRE(fakeGame.tick() == -1);  // TODO: toggle it to == 0, when the implementation is done.
    sleep(1);
    REQUIRE(fakeGame.tick() == -1);    // TODO: toggle it to == 0, when the implementation is done.
    REQUIRE(fakeGame.deinit() == -1);  // TODO: toggle it to == 0, when the implementation is done.
}
