#include <one/arcus/c_error.h>
#include <one/arcus/error.h>

#ifdef __cplusplus
extern "C" {
#endif

bool one_is_error(OneError err) {
    return i3d::one::is_error(err);
}

const char *one_error_text(OneError err) {
    return i3d::one::error_text(err);
}

#ifdef __cplusplus
};
#endif
