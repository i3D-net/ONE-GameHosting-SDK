#include "../catch2/catch.hpp"
#include "../../one/agent/agent.h"

using namespace one;

TEST_CASE("Agent standalone tests", "[agent]") {
    Agent agent = Agent("localhost", 9001);
    REQUIRE(agent.status() == 0);
}