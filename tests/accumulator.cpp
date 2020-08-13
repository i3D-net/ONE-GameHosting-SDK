#include <catch.hpp>
#include <one/arcus/internal/accumulator.h>

#include <string.h>
#include <cstring>

using namespace one;

TEST_CASE("accumulator", "[arcus]") {
    constexpr auto capacity = 8;
    Accumulator accumulator(capacity);
    REQUIRE(accumulator.capacity() == capacity);

    // Fill it up.
    const auto full = std::string("12345678");
    accumulator.put(full.data(), full.size());
    REQUIRE(accumulator.capacity() == capacity);
    REQUIRE(accumulator.size() == full.size());

    // Check data is good.
    char *data = nullptr;
    accumulator.peek(full.size(), reinterpret_cast<void **>(&data));
    REQUIRE(std::strncmp(data, full.data(), full.size()) == 0);

    // Remove all.
    accumulator.trim(full.size());
    REQUIRE(accumulator.capacity() == capacity);
    REQUIRE(accumulator.size() == 0);

    // Fill it half way.
    const auto quarter = std::string("12");
    const auto two_quarters = quarter + quarter;
    accumulator.put(quarter.data(), quarter.size());
    REQUIRE(accumulator.capacity() == capacity);
    REQUIRE(accumulator.size() == quarter.size());
    accumulator.put(quarter.data(), quarter.size());
    REQUIRE(accumulator.capacity() == capacity);
    REQUIRE(accumulator.size() == quarter.size() * 2);

    // Check data.
    accumulator.peek(two_quarters.size(), reinterpret_cast<void **>(&data));
    REQUIRE(std::strncmp(data, two_quarters.data(), two_quarters.size()) == 0);

    // Remove a quarter.
    accumulator.trim(quarter.size());
    REQUIRE(accumulator.capacity() == capacity);
    REQUIRE(accumulator.size() == quarter.size());

    // Check data.
    accumulator.peek(quarter.size(), reinterpret_cast<void **>(&data));
    REQUIRE(std::strncmp(data, quarter.data(), quarter.size()) == 0);
}
