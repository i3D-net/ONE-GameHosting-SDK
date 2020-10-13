#include <catch.hpp>

#include <one/arcus/internal/ring.h>

using namespace i3d::one;

TEST_CASE("ring", "[arcus]") {
    const size_t capacity = 2;
    Ring<int> ring(capacity);
    REQUIRE(ring.capacity() == capacity);

    REQUIRE(ring.peek() == nullptr);

    int i = 1;
    ring.push(i);
    REQUIRE(*ring.peek() == 1);
    REQUIRE(ring.pop() == 1);

    ring.push(i);
    i = 2;
    ring.push(i);

    REQUIRE(ring.pop() == 1);
    REQUIRE(ring.pop() == 2);

    // Do it again for first wrap-around test.
    i = 1;
    ring.push(i);
    i = 2;
    ring.push(i);
    REQUIRE(ring.pop() == 1);
    REQUIRE(ring.pop() == 2);

    // Overflow.
    i = 1;
    ring.push(i);
    i = 2;
    ring.push(i);
    i = 3;
    ring.push(i);
    i = 4;
    ring.push(i);
    REQUIRE(*ring.peek() == 3);
    REQUIRE(ring.pop() == 3);
    REQUIRE(ring.pop() == 4);
}
