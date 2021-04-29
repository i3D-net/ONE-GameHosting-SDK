#include <one/ping/c_error.h>
#include <one/ping/error.h>

#ifdef __cplusplus
extern "C" {
#endif

bool i3d_ping_is_error(I3dPingError err) {
    return i3d::ping::is_error(err);
}

const char *i3d_ping_error_text(I3dPingError err) {
    return i3d::ping::error_text(err);
}

#ifdef __cplusplus
};
#endif
