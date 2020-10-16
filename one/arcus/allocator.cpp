#include <one/arcus/allocator.h>

#include <assert.h>

namespace i3d {
namespace one {

namespace allocator {

namespace {

// Wrapper to allow assignment to the std::function.
void *default_alloc(size_t bytes) {
    return ::operator new(bytes);
}

// Wrapper to allow assignment to the std::function.
void default_free(void *p) {
    ::operator delete(p);
}

}  // namespace

// Global allocation overridable functions.
std::function<void *(size_t)> _alloc = default_alloc;
std::function<void(void *)> _free = default_free;

void set_alloc(std::function<void *(size_t)> fn) {
    _alloc = fn;
}

void set_free(std::function<void(void *)> fn) {
    _free = fn;
}

void reset_overrides() {
    _alloc = default_alloc;
    _free = default_free;
}

void *alloc(size_t bytes) {
    assert(_alloc);
    void *p = _alloc(bytes);
    assert(p != nullptr);
    return p;
}

void free(void *p) {
    assert(_free);
    _free(p);
}

}  // namespace allocator

}  // namespace one
}  // namespace i3d