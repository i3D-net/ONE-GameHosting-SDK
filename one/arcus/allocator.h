#pragma once

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

// Sets the allocators back to the default.
void reset_overrides();

// Use the function set by set_alloc to allocate memory.
void *alloc(size_t);

// Use the function set by set_free to free memory.
void free(void *);

// Equivalent to the new operator, but using the function set by set_alloc.
template <class T, class... Args>
T *create(Args &&...args) {
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