#pragma once

#include <stddef.h>

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

    // length must be less than or equal to capacity - size.
    void put(const char *data, size_t length);

    // Provides a pointer to data from the beginning of the stream. Sets the
    // given data pointer to the data and returns the number of bytes in the
    // data. length must be <= size.
    void peek(size_t length, char **data);

    // Drops the number of given bytes from the beginning of the stream, making
    // capacity at the end. length must be less than size.
    void trim(size_t length);

private:
    Accumulator() = delete;
    Accumulator(Accumulator &other) = delete;

    char *_buffer;
    size_t _capacity;
    size_t _size;
};

}  // namespace one
