#include <catch.hpp>
#include <util.h>
#include <one/arcus/error.h>
#include <one/agent/agent.h>
#include <one/arcus/client.h>
#include <one/arcus/error.h>

using namespace one;

TEST_CASE("Agent standalone life cycle", "[agent]") {
    Agent agent;
    REQUIRE(!is_error(agent.init("127.0.0.1", 19001)));
    REQUIRE(agent.update() == ONE_ERROR_SOCKET_CONNECT_FAILED);
    REQUIRE(agent.client().status() == Client::Status::connecting);
}
