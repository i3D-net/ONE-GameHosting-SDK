#include <one/arcus/c_api.h>

#include <one/arcus/arcus.h>

namespace one {
namespace {

int create_server(OneServerPtr* server) {
    if (server == nullptr) {
        return -1;
    }

    auto s = new Server();

    if (s == nullptr) {
        return -1;
    }

    *server = (OneServerPtr)s;
    return 0;
}

void destroy_server(OneServerPtr* server) {
    if (server == nullptr) {
        return;
    }

    auto s = (Server*)(*server);

    if (s != nullptr) {
        delete s;
        *server = nullptr;
    }
}

int update(OneServerPtr server) {
    auto s = (Server*)server;

    if (s == nullptr) {
        return -1;
    }

    return s->update();
}

int status(OneServerPtr const server) {
    auto s = (Server*)server;

    if (s == nullptr) {
        return -1;
    }

    return s->status();
}

int listen(OneServerPtr server, unsigned int port) {
    auto s = (Server*)server;

    if (s == nullptr) {
        return -1;
    }

    return s->listen(port);
}

int close(OneServerPtr server) {
    auto s = (Server*)server;

    if (s == nullptr) {
        return -1;
    }

    return s->close();
}

OneGameHostingApiPtr game_hosting_api() {
    static OneServerApi server_api;
    server_api.create = create_server;
    server_api.destroy = destroy_server;
    server_api.update = update;
    server_api.status = status;
    server_api.listen = listen;
    server_api.close = close;

    static OneGameHostingApi api;
    api.server_api = &server_api;
    return &api;
}

}  // Unnamed namespace.
}  // namespace one

extern "C" ONE_EXPORT OneGameHostingApiPtr ONE_STDCALL one_game_hosting_api() {
    return one::game_hosting_api();
};
