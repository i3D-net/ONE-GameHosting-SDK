#include <catch.hpp>
#include <util.h>

#include <one/arcus/c_error.h>
#include <one/arcus/error.h>

#include <string>

using namespace i3d::one;

TEST_CASE("error string description", "[error]") {
    std::string description = error_text(ONE_ERROR_NONE);

    if (description == "") {
        WARN("error does not have description set!");
    }

    REQUIRE(description != "");
}
