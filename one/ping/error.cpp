#include <one/ping/error.h>

#include <one/ping/allocator.h>

#include <unordered_map>

// Util that replaces the given symbol with a pair of it and its string
// representation.
#define I3D_PING_SYMBOL_STRING_PAIR(e) e, #e

namespace i3d {
namespace ping {

const char *error_text(OneError err) {
    static std::unordered_map<I3dPingError, const char *> lookup = {
        {I3D_PING_SYMBOL_STRING_PAIR(I3D_PING_ERROR_NONE)}};
    auto it = lookup.find(err);
    if (it == lookup.end()) {
        return "";
    }
    return it->second;
}

}  // namespace ping
}  // namespace i3d
