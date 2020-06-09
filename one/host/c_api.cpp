#include "c_api.h"

#include "internal/host.h"

namespace one {
namespace {

OneGameHostPtr create_host() {
    auto c = new Host();
    return (OneGameHostPtr)c;
}

void destroy_host(OneGameHostPtr host) {
    auto h = (Host*)host;

    if (h != nullptr) {
        delete h;
    }
}

OneGameHostApiPtr game_host_api() {
    static OneGameHostApi api;
    api.create_host = create_host;
    api.destroy_host = destroy_host;
    return &api;
}

}  // Unnamed namespace.
}  // namespace one

extern "C" ONE_EXPORT OneGameHostApiPtr ONE_STDCALL one_game_host_api() {
    return one::game_host_api();
};
