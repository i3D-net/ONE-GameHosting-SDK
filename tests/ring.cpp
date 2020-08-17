#include <catch.hpp>

#include <one/arcus/internal/ring.h>

using namespace one;

TEST_CASE("ring", "[arcus]") {
    const size_t capacity = 2;
    Ring<int> ring(capacity);
    REQUIRE(ring.capacity() == capacity);

    ring.push(1);
    REQUIRE(ring.pop() == 1);

    ring.push(1);
    ring.push(2);
    REQUIRE(ring.pop() == 1);
    REQUIRE(ring.pop() == 2);

    // Do it again for first wrap-around test.
    ring.push(1);
    ring.push(2);
    REQUIRE(ring.pop() == 1);
    REQUIRE(ring.pop() == 2);

    // Overflow.
    ring.push(1);
    ring.push(2);
    ring.push(3);
    ring.push(4);
    REQUIRE(ring.pop() == 3);
    REQUIRE(ring.pop() == 4);
}