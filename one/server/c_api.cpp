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

OneGameHostingApiPtr game_hosting_api() {
    static OneHostApi host_api;
    host_api.create = create_host;
    host_api.destroy = destroy_host;

    static OneGameHostingApi api;
    api.host_api = &host_api;
    return &api;
}

}  // Unnamed namespace.
}  // namespace one

extern "C" ONE_EXPORT OneGameHostingApiPtr ONE_STDCALL one_game_hosting_api() {
    return one::game_hosting_api();
};
