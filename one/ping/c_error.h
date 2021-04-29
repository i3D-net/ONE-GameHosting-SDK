#pragma once

/** @file c_error.h
    @brief The C API error values and utilities.
**/

#include <one/ping/c_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

// Note - when adding errors:
//    1. Never change an existing number mapping.
//    2. Add the new error symbol to the error_text function in error.cpp.
typedef enum I3dPingError {
    I3D_PING_ERROR_NONE = 0,
} I3dPingError;

I3D_PING_EXPORT bool i3d_ping_is_error(I3dPingError err);

// Given a I3dPingError, returns a string matching the symbol name, e.g.
// i3d_ping_error_text(I3D_PING_ERROR_CONNECTION_INVALID_MESSAGE_HEADER) will return
// "I3D_PING_ERROR_CONNECTION_INVALID_MESSAGE_HEADER".
I3D_PING_EXPORT const char *i3d_ping_error_text(I3dPingError err);

#ifdef __cplusplus
};
#endif
