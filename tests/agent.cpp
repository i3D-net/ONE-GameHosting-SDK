#include <catch.hpp>
#include <util.h>
#include <one/arcus/error.h>
#include <one/agent/agent.h>
#include <one/arcus/client.h>
#include <one/arcus/error.h>

using namespace one;

TEST_CASE("Agent standalone life cycle", "[agent]") {
    Agent agent;
    REQUIRE(agent.connect("127.0.0.1", 19001) == -1);
    REQUIRE(agent.client().status() == Client::Status::handshake);
}
