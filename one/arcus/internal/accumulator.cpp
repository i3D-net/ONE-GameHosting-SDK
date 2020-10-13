#include <one/arcus/internal/accumulator.h>

#include <assert.h>
#include <cstring>

namespace i3d {
namespace one {

Accumulator::Accumulator(size_t capacity) : _capacity(capacity), _size(0) {
    _buffer = new char[capacity]();
}

Accumulator::~Accumulator() {
    if (_buffer != nullptr) {
        delete[] _buffer;
        _buffer = nullptr;
    }
}

void Accumulator::put(const void *data, size_t length) {
    if (_buffer == nullptr) {
        return;
    }
    if (length == 0) {
        return;
    }
    assert(_size + length <= _capacity);
    memcpy(_buffer + _size, data, length);
    _size += length;
}

void Accumulator::peek(size_t length, void **data) {
    if (_buffer == nullptr) {
        return;
    }
    assert(data);
    assert(length <= _size);
    *data = _buffer;
}

void Accumulator::trim(size_t length) {
    if (_buffer == nullptr) {
        return;
    }
    if (length == 0) {
        return;
    }
    assert(length <= _size);
    memmove(_buffer, _buffer + length, _size - length);
    _size -= length;
}

void Accumulator::get(size_t length, void **data) {
    peek(length, data);
    trim(length);
}

}  // namespace one
}  // namespace i3d
