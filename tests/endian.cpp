#include <catch.hpp>

#include <one/arcus/platform.h>
#include <one/arcus/internal/endian.h>

using namespace i3d::one;

// Assume windows is little endian. No Linux test is provided since Linux
// may be bix endian (e.g. if running in WSL2 on Windows).
#ifdef ONE_WINDOWS
TEST_CASE("endian windows", "[arcus]") {
    REQUIRE(endian::which() == endian::Arch::little);
}
#endif

#ifndef ONE_WINDOWS
TEST_CASE("endian linux", "[arcus]") {
    REQUIRE(endian::which() == endian::Arch::little);
}
#endif
