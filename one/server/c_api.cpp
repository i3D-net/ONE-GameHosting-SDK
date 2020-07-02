#include "c_api.h"

#include "internal/server.h"

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

}  // Unnamed namespace.
}  // namespace one

extern "C" {

OneServerPtr one_server_create(void) {
    return one::create_server();
}

void one_server_destroy(OneServerPtr server) {
    one::destroy_server(server);
}

}; // extern "C"
