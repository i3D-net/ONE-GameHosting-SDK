#include "c_api.h"

#include "internal/host.h"

namespace one {
namespace {

OneServerPtr create_server() {
    auto c = new Server();
    return (OneServerPtr)c;
}

void destroy_server(OneServerPtr server) {
    auto s = (Server*)server;

    if (s != nullptr) {
        delete s;
    }
}

OneGameHostingApiPtr game_hosting_api() {
    static OneServerApi server_api;
    server_api.create = create_server;
    server_api.destroy = destroy_server;

    static OneGameHostingApi api;
    api.server_api = &server_api;
    return &api;
}

}  // Unnamed namespace.
}  // namespace one

extern "C" ONE_EXPORT OneGameHostingApiPtr ONE_STDCALL one_game_hosting_api() {
    return one::game_hosting_api();
};
