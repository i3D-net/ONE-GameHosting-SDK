#include <one/arcus/c_api.h>

#include <one/arcus/server.h>
#include <one/arcus/message.h>
#include <one/arcus/opcode.h>

#include <utility>
#include <string>

namespace one {
namespace {

int message_create(OneMessagePtr *message) {
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

void message_destroy(OneMessagePtr *message) {
    if (message == nullptr) {
        return;
    }

    auto m = (Message *)(*message);
    if (m != nullptr) {
        delete m;
        *message = nullptr;
    }
}

int message_init(OneMessagePtr message, int code, const char *data, unsigned int size) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    return m->init(static_cast<Opcode>(code), {data, size});
}

int message_code(OneMessagePtr message, int *code) {
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

int message_val_int(OneMessagePtr message, const char *key, int *val) {
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

int message_val_string(OneMessagePtr message, const char *key, char **val) {
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

int message_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
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

int message_val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
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

int message_set_val_int(OneMessagePtr message, const char *key, int val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    if (key == nullptr) {
        return -1;
    }

    return m->payload().set_val_int(key, val);
}

int message_set_val_string(OneMessagePtr message, const char *key, const char *val) {
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

int message_set_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
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

int message_set_val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
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

int message_prepare_error_response(OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    return messages::prepare_error_response(*m);
}

int message_prepare_live_state_response(int player, int max_player, const char *name, const char *map,
                                const char *mode, const char *version, OneMessagePtr message) {
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

    return messages::prepare_live_state_response(player, max_player, name, map, mode, version, *m);
}

int message_prepare_host_information_request(OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    return messages::prepare_host_information_request(*m);
}

int server_create(size_t max_message_in, size_t max_message_out, OneServerPtr *server) {
    if (server == nullptr) {
        return -1;
    }

    auto s = new Server();
    if (s == nullptr) {
        return -1;
    }

    int error = s->init(max_message_in, max_message_out);
    if (error != 0) {
        delete s;
        return -1;
    }

    *server = (OneServerPtr)s;
    return 0;
}

void server_destroy(OneServerPtr *server) {
    if (server == nullptr) {
        return;
    }

    auto s = (Server *)(*server);
    if (s != nullptr) {
        delete s;
        *server = nullptr;
    }
}

int server_update(OneServerPtr server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    return s->update();
}

int server_status(OneServerPtr const server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    return static_cast<int>(s->status());
}

int server_listen(OneServerPtr server, unsigned int port, int queueLength) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    return s->listen(port, queueLength);
}

int server_shutdown(OneServerPtr server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    return s->shutdown();
}

int server_send_error_response(OneServerPtr server, OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    return s->send_error_response(*m);
}

int server_send_live_state_response(OneServerPtr server, OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    return s->send_live_state_response(*m);
}

int server_send_host_information_request(OneServerPtr server, OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return -1;
    }

    return s->send_host_information_request(*m);
}

int server_set_soft_stop_callback(OneServerPtr server, void (*callback)(void *, int), void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    if (callback == nullptr) {
        return -1;
    }

    s->set_soft_stop_callback(callback, data);
    return 0;
}

int server_set_allocated_callback(OneServerPtr server, void (*callback)(void *, void *), void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    if (callback == nullptr) {
        return -1;
    }

    s->set_allocated_callback(callback, data);
    return 0;
}

int server_set_meta_data_callback(OneServerPtr server, void (*callback)(void *, void *), void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    if (callback == nullptr) {
        return -1;
    }

    s->set_meta_data_callback(callback, data);
    return 0;
}

int server_set_live_state_request_callback(OneServerPtr server, void (*callback)(void *), void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return -1;
    }

    if (callback == nullptr) {
        return -1;
    }

    s->set_live_state_request_callback(callback, data);
    return 0;
}

void allocator_set_alloc(void *(callback)(unsigned int size)) {
    // Todo: implement.

}

void allocator_set_free(void(callback)(void *)) {
    // Todo: implement.
}

}  // Unnamed namespace.
}  // namespace one

extern "C" {

int one_message_create(OneMessagePtr *message) {
    return one::message_create(message);
}

void one_message_destroy(OneMessagePtr *message) {
    return one::message_destroy(message);
}

int one_message_init(OneMessagePtr message, int code, const char *data, unsigned int size) {
    return one::message_init(message, code, data, size);
}

int one_message_code(OneMessagePtr message, int *code) {
    return one::message_code(message, code);
}

int one_message_val_int(OneMessagePtr message, const char *key, int *val) {
    return one::message_val_int(message, key, val);
}

int one_message_val_string(OneMessagePtr message, const char *key, char **val) {
    return one::message_val_string(message, key, val);
}

int one_message_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    return one::message_val_array(message, key, val);
}

int one_message_val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
    return one::message_val_object(message, key, val);
}

int one_message_set_val_int(OneMessagePtr message, const char *key, int val) {
    return one::message_set_val_int(message, key, val);
}

int one_message_set_val_string(OneMessagePtr message, const char *key, const char *val) {
    return one::message_set_val_string(message, key, val);
}

int one_message_set_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    return one::message_set_val_array(message, key, val);
}

int one_message_set_val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
    return one::message_set_val_object(message, key, val);
}

int one_message_prepare_error_response(OneMessagePtr message) {
    return one::message_prepare_error_response(message);
}

int one_message_prepare_live_state_response(int player, int max_player, const char *name,
                                            const char *map, const char *mode, const char *version,
                                            OneMessagePtr message) {
    return one::message_prepare_live_state_response(player, max_player, name, map, mode, version, message);
}

int one_message_prepare_host_information_request(OneMessagePtr message) {
    return one::message_prepare_host_information_request(message);
}

int one_server_create(size_t max_message_in, size_t max_message_out, OneServerPtr *server) {
    return one::server_create(max_message_in, max_message_out, server);
}

void one_server_destroy(OneServerPtr *server) {
    return one::server_destroy(server);
}

int one_server_update(OneServerPtr server) {
    return one::server_update(server);
}

int one_server_status(OneServerPtr const server) {
    return one::server_status(server);
}

int one_server_listen(OneServerPtr server, unsigned int port, int queueLength) {
    return one::server_listen(server, port, queueLength);
}

int one_server_shutdown(OneServerPtr server) {
    return one::server_shutdown(server);
}

int one_server_send_error_response(OneServerPtr server, OneMessagePtr message) {
    return one::server_send_error_response(server, message);
}

int one_server_send_live_state_response(OneServerPtr server, OneMessagePtr message) {
    return one::server_send_live_state_response(server, message);
}

int one_server_send_host_information_request(OneServerPtr server, OneMessagePtr message) {
    return one::server_send_host_information_request(server, message);
}

int one_server_set_soft_stop_callback(OneServerPtr server, void (*callback)(void *data, int timeout),
                                      void *data) {
    return one::server_set_soft_stop_callback(server, callback, data);
}

int one_server_set_allocated_callback(OneServerPtr server, void (*callback)(void *data, void *array),
                                      void *data) {
    return one::server_set_allocated_callback(server, callback, data);
}

int one_server_set_meta_data_callback(OneServerPtr server, void (*callback)(void *data, void *array),
                                      void *data) {
    return one::server_set_meta_data_callback(server, callback, data);
}

int one_server_set_live_state_request_callback(OneServerPtr server, void (*callback)(void *data),
                                               void *data) {
    return one::server_set_live_state_request_callback(server, callback, data);
}

void one_allocator_set_alloc(void *(callback)(unsigned int size)) {
    one::allocator_set_alloc(callback);

}

void one_allocator_set_free(void(callback)(void *)) {
    one::allocator_set_free(callback);
}

}; // extern "C"

