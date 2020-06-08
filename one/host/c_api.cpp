#include "c_api.h"

#include "internal/host.h"

namespace one {
namespace {

OneGameHostPtr create_client() {
    auto c = new Client();
    return (OneGameHostPtr)c;
}
void destroy_client(OneGameHostPtr host) {
    Client* client = (Client*)host;
    // Assert not null.
    delete client;
}

OneGameHostApiPtr game_host_api() {
    static OneGameHostApi api;
    api.create_client = create_client;
    return &api;
}

}  // Unnamed namespace.
}  // namespace one

extern "C" ONE_EXPORT OneGameHostApiPtr ONE_STDCALL one_game_host_api() {
    return one::game_host_api();
};
