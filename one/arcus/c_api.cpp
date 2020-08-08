#include <one/arcus/c_api.h>

#include <one/arcus/arcus.h>

namespace one {
namespace {

int create_server(OneServerPtr *server) {
    if (server == nullptr) {
        return -1;
    }

    auto s = new Server();
    if (s == nullptr) {
        return -1;
    }

    int error = s->init();
    if (error != 0) {
        delete s;
        return -1;
    }

    *server = (OneServerPtr)s;
    return 0;
}

void destroy_server(OneServerPtr *server) {
    if (server == nullptr) {
        return;
    }

    auto s = (Server *)(*server);
    if (s != nullptr) {
        delete s;
        *server = nullptr;
    }
}

int update(OneServerPtr server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    return s->update();
}

int status(OneServerPtr const server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    return s->status();
}

int listen(OneServerPtr server, unsigned int port) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    return s->listen(port);
}

int shutdown(OneServerPtr server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    return s->shutdown();
}

int send_server_info(OneServerPtr server, OneMessagePtr data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    auto m = (Message *)data;
    if (m == nullptr) {
        return -1;
    }

    return s->send_server_info(*m);
}

int set_soft_stop_callback(OneServerPtr server, void (*cb)(int)) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    if (cb == nullptr) {
        return -1;
    }

    s->set_soft_stop_callback(cb);
    return 0;
}

int set_live_state_request_callback(OneServerPtr server, void (*cb)(void)) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    if (cb == nullptr) {
        return -1;
    }

    s->set_live_state_request_callback(cb);
    return 0;
}

OneGameHostingApiPtr game_hosting_api() {
    static OneServerApi server_api;
    server_api.create = create_server;
    server_api.destroy = destroy_server;
    server_api.update = update;
    server_api.status = status;
    server_api.listen = listen;
    server_api.shutdown = shutdown;
    server_api.send_server_info = send_server_info;
    server_api.set_soft_stop_callback = set_soft_stop_callback;
    server_api.set_live_state_request_callback = set_live_state_request_callback;

    static OneGameHostingApi api;
    api.server_api = &server_api;
    return &api;
}

}  // Unnamed namespace.
}  // namespace one

extern "C" ONE_EXPORT OneGameHostingApiPtr ONE_STDCALL one_game_hosting_api() {
    return one::game_hosting_api();
};
