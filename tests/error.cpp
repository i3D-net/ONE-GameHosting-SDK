#include <catch.hpp>
#include <util.h>

#include <one/arcus/c_error.h>
#include <one/arcus/error.h>

#include <string>

using namespace i3d::one;

TEST_CASE("error string description", "[error]") {
    std::string description;
    int counter = 0;
    for (; counter < ONE_ERROR_COUNT; ++counter) {

        description = error_text(static_cast<Error>(counter));

        if (description == "") {
            WARN("error( " << counter << " )does not have description set!");
        }

        REQUIRE(description != "");
    }

    REQUIRE(counter == ONE_ERROR_COUNT);
}
