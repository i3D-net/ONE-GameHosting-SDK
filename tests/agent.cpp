#include <catch.hpp>
#include <util.h>
#include <one/arcus/error.h>
#include <one/agent/agent.h>
#include <one/arcus/arcus.h>
#include <one/arcus/error.h>

using namespace one;

TEST_CASE("Agent standalone connection tests", "[agent]") {
    Agent agent;
    REQUIRE(agent.connect("127.0.0.1", 9001) == 0);
    REQUIRE(agent.status() == 0);
    for_sleep(5, 1, [&]() { return agent.update() == 0; });
    REQUIRE(agent.client().status() == Client::Status::ready);
}

TEST_CASE("Agent connection failed tests", "[agent]") {
    Agent agent;
    REQUIRE(agent.connect("invalid", -1) == 0);  // FIXME when the agent connect is implemented.
    REQUIRE(agent.status() == 0);                // FIXME when the agent status is implemented.
}
