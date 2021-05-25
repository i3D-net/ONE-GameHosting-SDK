#pragma once

#include <cstdlib>

namespace i3d_ping_integration {

// Provides example global allocation hooks used to override the allocations.
namespace allocation {

void *alloc(unsigned int bytes) {
    return std::malloc(bytes);
}

void free(void *p) {
    std::free(p);
}

void *realloc(void *p, unsigned int bytes) {
    return std::realloc(p, bytes);
}

}  // namespace allocation

}  // namespace i3d_ping_integration
