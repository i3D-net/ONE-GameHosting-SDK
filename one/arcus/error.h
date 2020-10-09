#pragma once

#include <one/arcus/c_error.h>

// C++ extensions and utilities for the c_error.h symbols.

namespace i3d {
namespace one {

typedef OneError Error;

const char *error_text(OneError);

constexpr bool is_error(OneError err) {
    return err != ONE_ERROR_NONE;
}

}  // namespace one
}  // namespace i3d