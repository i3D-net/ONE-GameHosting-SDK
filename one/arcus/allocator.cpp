#include <one/arcus/allocator.h>

#include <assert.h>
#include <cstdlib>

namespace i3d {
namespace one {

namespace allocator {

namespace {

// Wrapper to allow assignment to the std::function.
void *default_alloc(size_t bytes) {
    return std::malloc(bytes);
}

// Wrapper to allow assignment to the std::function.
void default_free(void *p) {
    std::free(p);
}

void *default_realloc(void *p, size_t bytes) {
    return std::realloc(p, bytes);
}

}  // namespace

// Global allocation overridable functions.
std::function<void *(size_t)> _alloc = default_alloc;
std::function<void(void *)> _free = default_free;
std::function<void *(void *, size_t)> _realloc = default_realloc;

void set_alloc(std::function<void *(size_t)> fn) {
    _alloc = fn;
}

void set_free(std::function<void(void *)> fn) {
    _free = fn;
}

void set_realloc(std::function<void *(void *, size_t)> fn) {
    _realloc = fn;
}

void reset_overrides() {
    _alloc = default_alloc;
    _free = default_free;
    _realloc = default_realloc;
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

void *realloc(void *p, size_t s) {
    return _realloc(p, s);
}

}  // namespace allocator

}  // namespace one
}  // namespace i3d
