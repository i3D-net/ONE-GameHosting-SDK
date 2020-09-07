#include <catch.hpp>

#include <one/arcus/internal/endian.h>

// Assume windows is little endian. No Linux test is provided since Linux
// may be bix endian (e.g. if running in WSL2 on Windows).
#ifdef WINDOWS
TEST_CASE("endian windows", "[arcus]") {
    REQUIRE(one::endian::which() == one::endian::Arch::little);
}
#endif

#ifndef WINDOWS
TEST_CASE("endian linux", "[arcus]") {
    REQUIRE(one::endian::which() == one::endian::Arch::little);
}
#endif
