#include <one/fake/ping/ping.h>

#include <assert.h>
#include <ctime>
#include <string>
#include <sstream>

#include <one/fake/ping/log.h>

namespace i3d_ping_integration {

// Provides example global allocation hooks used to override the allocations
// made within the ONE Game Hosting SDK.
namespace allocation {

// For debug purposes.
size_t _alloc_count = 0;  // Including calls to realloc with a null ptr.
size_t _free_count = 0;

size_t alloc_count() {
    return _alloc_count;
}

size_t free_count() {
    return _free_count;
}

// A custom memory alloc example.
void *alloc(size_t bytes) {
    _alloc_count++;
    return std::malloc(bytes);
}

// A custom memory free example.
void free(void *p) {
    // For counting purposes, exclude calls to deleting a nullptr. Passing it on
    // anyway to delete just as an example that it is safe to do so, although
    // there is no need to.
    if (p != nullptr) {
        _free_count++;
    }
    std::free(p);
}

void *realloc(void *p, size_t bytes) {
    if (p == nullptr) {
        _alloc_count++;
    }
    return std::realloc(p, bytes);
}

}  // namespace allocation

Ping::Ping()
    :  //_i3d_ping_client()  // The i3D Ping Client's default constructor is called.
    _ping()
    , _quiet(false) {}

Ping::~Ping() {
    //_i3d_ping_client.shutdown();
}

bool Ping::init() {
    const std::lock_guard<std::mutex> lock(_ping);
    return true;
}

void Ping::shutdown() {
    const std::lock_guard<std::mutex> lock(_ping);
    //_i3d_ping_client.shutdown();
}

void Ping::update() {
    const std::lock_guard<std::mutex> lock(_ping);
    //_i3d_ping_client.update(_quiet);
}

}  // namespace i3d_ping_integration
