#include <catch.hpp>

#include <functional>

#include <one/arcus/internal/codec.h>

using namespace one;

TEST_CASE("hello", "[codec]") {
    {
        auto hello = codec::valid_hello();
        REQUIRE(validate_hello(hello));
    }
    {
        auto hello = codec::valid_hello();
        hello.version = (char)0x0;
        REQUIRE(!validate_hello(hello));
    }
    {
        auto hello = codec::valid_hello();
        hello.dummy[0] = (char)0x1;
        REQUIRE(!validate_hello(hello));
    }
    {
        auto hello = codec::valid_hello();
        hello.id[0] = (char)0x0;
        REQUIRE(!validate_hello(hello));
    }
}

// No Header validation in the Codec.