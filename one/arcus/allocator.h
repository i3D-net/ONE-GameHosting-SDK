#pragma once

#include <assert.h>
#include <functional>

namespace i3d {
namespace one {

namespace allocator {

// Override alloc, which is used by all allocation functions in this
// namespace. Default is ::operator new.
void set_alloc(std::function<void *(size_t)>);

// Override free, which is used by all deallocation functions in this
// namespace. Default is ::operator delete.
void set_free(std::function<void(void *)>);

// Override realloc, which is used by the realloc function in this
// namespace. Default is ::operator delete.
void set_realloc(std::function<void *(void *, size_t)>);

// Sets the allocators back to the default.
void reset_overrides();

// Use the function set by set_alloc to allocate memory.
void *alloc(size_t);

// Use the function set by set_free to free memory.
void free(void *);

// Reallocate existing memory with new size. Must match standard c behavior.
void *realloc(void *p, size_t s);

// Equivalent to the new operator, but using the function set by set_alloc.
template <class T, class... Args>
T *create(Args &&... args) {
    auto data = alloc(sizeof(T));
    auto p = reinterpret_cast<T *>(data);
    ::new (p) T(std::forward<Args>(args)...);
    return p;
}

// Equivalent to the delete operator, but using the function set by set_free.
template <class T>
void destroy(T *p) noexcept {
    p->~T();
    free(p);
}

// Allocates an array using the function set by set_alloc. Each array element
// has its constructor called, with the same arguments. The returned memory must
// be deleted via destroy_array.
template <class T, class... Args>
T *create_array(size_t count, Args &&... args) {
    // Allocate additional space to store the array length.
    size_t padding = sizeof(size_t);
    auto data = alloc(sizeof(T) * count + padding);

    // The padding will be at the front of the data. Set it to the array length.
    size_t *start = reinterpret_cast<size_t *>(data);
    *start = count;

    // The actual array elements start directly after the padding.
    auto p = reinterpret_cast<T *>(start + 1);
    auto element = p;
    for (size_t i = 0; i < count; ++i) {
        ::new (element) T(std::forward<Args>(args)...);
        ++element;
    }

    return p;
}

// Deallocates an array using the function set by set_free. Each array element
// has its destructor called. The destroyed memory must have been allocated via
// create_array.
template <class T>
void destroy_array(T *p) noexcept {
    assert(p);

    // The buffer allocated will start before the array length preceding the
    // array elements (p).
    size_t *start = (reinterpret_cast<size_t *>(p) - 1);
    // The array length is first.
    size_t count = *start;

    // Call destructors.
    T *element = reinterpret_cast<T *>(p);
    for (size_t i = 0; i < count; ++i) {
        element->~T();
        ++element;
    }

    // Free the original buffer including array length.
    free(start);
}

}  // namespace allocator

// Custom c++ 11 std::allocator to use for all std types.
template <class T>
class StandardAllocator {
public:
    using value_type = T;

    StandardAllocator() noexcept {}
    template <class U>
    StandardAllocator(StandardAllocator<U> const &other) noexcept {}

    value_type *  // Use pointer if pointer is not a value_type*
    allocate(std::size_t n) {
        return static_cast<value_type *>(allocator::alloc(n * sizeof(value_type)));
    }

    void deallocate(value_type *p,
                    std::size_t) noexcept  // Use pointer if pointer is not a value_type*
    {
        allocator::free(p);
    }
};

template <class T, class U>
bool operator==(StandardAllocator<T> const &, StandardAllocator<U> const &) noexcept {
    return true;
}

template <class T, class U>
bool operator!=(StandardAllocator<T> const &x, StandardAllocator<U> const &y) noexcept {
    return !(x == y);
}

}  // namespace one
}  // namespace i3d
