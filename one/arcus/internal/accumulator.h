#pragma once

#include <stddef.h>

namespace i3d {
namespace one {

// Accumulator is fixed-size buffer for accumulating byte data.
// It adds new data to the end, and removes data from the front.
class Accumulator final {
public:
    Accumulator(size_t capacity);
    ~Accumulator();

    size_t capacity() const {
        return _capacity;
    }
    size_t size() const {
        return _size;
    }

    void clear() {
        trim(size());
    };

    // Copies the given data and adds it to the stream. length must be less than
    // or equal to capacity - size.
    void put(const void *data, size_t length);

    // Provides a pointer to data from the beginning of the stream. Sets the
    // given data pointer to the data. length must be <= size.
    void peek(size_t length, void **data);

    // Drops the number of given bytes from the beginning of the stream, making
    // capacity at the end. length must be less than size.
    void trim(size_t length);

    // Get is a util equivalent to peek + trim.
    void get(size_t length, void **data);

private:
    Accumulator() = delete;
    Accumulator(Accumulator &other) = delete;

    char *_buffer;
    size_t _capacity;
    size_t _size;
};

}  // namespace one
}  // namespace i3d