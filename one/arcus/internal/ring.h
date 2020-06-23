#pragma once

namespace one {

// FIFO ring buffer with a fixed capacity.
template<typename T>
class RingBuffer {
public:
    RingBuffer(size_t capacity);

    size_t capacity() const;
    size_t size() const;

    void push(T);
    T pop();

private:
    // Todo: some reasonable storage buffer type, allocator overriding.
    //std::vector<T>	_buffer;
    //unsigned int _next;
    //size_t _size;
};


} // namespace one