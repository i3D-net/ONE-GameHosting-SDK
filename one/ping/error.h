#pragma once

#include <one/ping/c_error.h>

// C++ extensions and utilities for the c_error.h symbols.

namespace i3d {
namespace ping {

const char *error_text(I3dPingError);

constexpr bool is_error(I3dPingError err) {
    return err != I3D_PING_ERROR_NONE;
}

}  // namespace ping
}  // namespace i3d