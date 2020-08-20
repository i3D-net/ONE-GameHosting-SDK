#include <catch.hpp>
#include <util.h>

#include <one/arcus/error.h>
#include <one/game/game.h>

using namespace one;

TEST_CASE("fake game life cycle", "[fake game]") {
    Game fakeGame(19001, 1, 10, 54, "test game", "test map", "test mode", "test version");
    REQUIRE(fakeGame.init(1024, 1024) == 0);
    REQUIRE(fakeGame.shutdown() == 0);
}
