#pragma once

#include <assert.h>

namespace one {

// FIFO ring buffer with a fixed capacity.
template <typename T>
class Ring final {
public:
    Ring(size_t capacity) : _buffer(nullptr), _capacity(capacity), _last(0), _next(0), _size(0) {
        assert(capacity > 0);
        _buffer = new T[capacity];
    }
    ~Ring() {
        assert(_buffer);
        delete _buffer;
        _buffer = nullptr;
    }

    Ring(const Ring &) = delete;
    Ring &operator=(const Ring &) = delete;

    void clear() {
        _next = _last = 0;
        _size = 0;
    }

    size_t capacity() const {
        return _capacity;
    }
    size_t size() const {
        return _size;
    }

    void push(const T &val) {
        _buffer[_next] = val;
        _next++;
        if (_next >= _capacity) _next = 0;
        if (_size < _capacity) _size++;
    }

    // Pops the oldest pushed value. Asserts if size is zero.
    const T &pop() {
        assert(_size > 0);
        // Cache the current last before incrementing so that the new
        // last can be error checked against the front of the ring, before
        // referencing the current last in the buffer.
        const auto prev_last = _last;
        _last++;
        if (_last >= _capacity) _last = 0;
        if (_size > 0) _size--;
        return _buffer[prev_last];
    }

private:
    T *_buffer;

    size_t _capacity;
    size_t _size;

    unsigned int _last;  // The oldest pushed item that is not yet popped.
    unsigned int _next;  // The most recent pushed item.
};

}  // namespace one
