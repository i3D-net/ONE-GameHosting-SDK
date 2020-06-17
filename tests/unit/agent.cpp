#include "../catch2/catch.hpp"
#include "../../one/agent/client.h"

using namespace one;

TEST_CASE("Agent standalone tests", "[agent]") {
    Client client = Client("localhost", 9001);
    REQUIRE(client.status() == 0);
}