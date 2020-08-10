#include <one/arcus/c_api.h>

#include <one/arcus/arcus.h>
#include <one/arcus/message.h>

#include <utility>
#include <string>

namespace one {
namespace {

int create_message(OneMessagePtr *message) {
    if (message == nullptr) {
        return -1;
    }

    auto m = new Message();
    if (m == nullptr) {
        return -1;
    }

    *message = (OneMessagePtr)m;
    return 0;
}

void destroy_message(OneMessagePtr *message) {
    if (message == nullptr) {
        return;
    }

    auto m = (Message *)(message);
    if (m != nullptr) {
        delete m;
        *message = nullptr;
    }
}

int init_message(OneMessagePtr message, int code, const char *data, unsigned int size) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    return m->init(code, {data, size});
}

int code_message(OneMessagePtr message, int *code) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (code == nullptr) {
        return -1;
    }

    *code = static_cast<int>(m->code());
    return 0;
}

int val_int(OneMessagePtr message, const char *key, int *val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (key == nullptr) {
        return -1;
    }

    if (val == nullptr) {
        return -1;
    }

    return m->payload().val_int(key, *val);
}

int val_string(OneMessagePtr message, const char *key, char **val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (key == nullptr) {
        return -1;
    }

    if (val == nullptr) {
        return -1;
    }

    if (val == nullptr) {
        return -1;
    }

    int error = m->payload().val_string(key, val);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (key == nullptr) {
        return -1;
    }

    auto a = (Array *)val;
    if (a == nullptr) {
        return -1;
    }

    return m->payload().val_array(key, *a);
}

int val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (key == nullptr) {
        return -1;
    }

    auto o = (Object *)val;
    if (o == nullptr) {
        return -1;
    }

    return m->payload().val_object(key, *o);
}

int set_val_int(OneMessagePtr message, const char *key, int val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (key == nullptr) {
        return -1;
    }

    return m->payload().set_val_int(key, val);
}

int set_val_string(OneMessagePtr message, const char *key, const char *val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (key == nullptr) {
        return -1;
    }

    if (val == nullptr) {
        return -1;
    }

    return m->payload().set_val_string(key, val);
}

int set_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (key == nullptr) {
        return -1;
    }

    auto a = (Array *)val;
    if (a == nullptr) {
        return -1;
    }

    return m->payload().set_val_array(key, *a);
}

int set_val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (key == nullptr) {
        return -1;
    }

    auto o = (Object *)val;
    if (o == nullptr) {
        return -1;
    }

    return m->payload().set_val_object(key, *o);
}

int prepare_live_state(OneMessagePtr message, int player, int max_player, const char *name,
                       const char *map, const char *mode, const char *version) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (name == nullptr) {
        return -1;
    }

    if (map == nullptr) {
        return -1;
    }

    if (mode == nullptr) {
        return -1;
    }

    if (version == nullptr) {
        return -1;
    }

    return messages::prepare_live_state(player, max_player, name, map, mode, version, *m);
}

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

int send_live_state(OneServerPtr server, OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    return s->send_live_state(*m);
}

int set_soft_stop_callback(OneServerPtr server, void (*cb)(void *, int), void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    if (cb == nullptr) {
        return -1;
    }

    s->set_soft_stop_callback(cb, data);
    return 0;
}

int set_live_state_request_callback(OneServerPtr server, void (*cb)(void *), void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    if (cb == nullptr) {
        return -1;
    }

    s->set_live_state_request_callback(cb, data);
    return 0;
}

OneGameHostingApiPtr game_hosting_api() {
    static OneMessageApi message_api;
    message_api.create = create_message;
    message_api.destroy = destroy_message;
    message_api.init = init_message;
    message_api.code = code_message;
    message_api.val_int = val_int;
    message_api.val_string = val_string;
    message_api.val_array = val_array;
    message_api.val_object = val_object;

    static OneMessagePrepareApi message_prepare_api;
    message_prepare_api.prepare_live_state = prepare_live_state;

    static OneServerApi server_api;
    server_api.create = create_server;
    server_api.destroy = destroy_server;
    server_api.update = update;
    server_api.status = status;
    server_api.listen = listen;
    server_api.shutdown = shutdown;
    server_api.send_live_state = send_live_state;
    server_api.set_soft_stop_callback = set_soft_stop_callback;
    server_api.set_live_state_request_callback = set_live_state_request_callback;

    static OneGameHostingApi api;
    api.message_api = &message_api;
    api.message_prepare_api = &message_prepare_api;
    api.server_api = &server_api;
    return &api;
}

}  // Unnamed namespace.
}  // namespace one

extern "C" ONE_EXPORT OneGameHostingApiPtr ONE_STDCALL one_game_hosting_api() {
    return one::game_hosting_api();
};
