#include <catch.hpp>

#include <one/arcus/c_api.h>
#include <one/arcus/c_error.h>

TEST_CASE("shared library c api", "[shared_lib]") {
    OneServerPtr server;
    auto err = one_server_create(nullptr, &server);
    REQUIRE(!one_is_error(err));
    REQUIRE(server != nullptr);
    one_server_destroy(server);
}