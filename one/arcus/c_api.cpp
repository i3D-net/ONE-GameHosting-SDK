#include <one/arcus/c_api.h>

#include <one/arcus/error.h>
#include <one/arcus/message.h>
#include <one/arcus/opcode.h>
#include <one/arcus/server.h>

#include <utility>
#include <string>

namespace one {
namespace {

Error message_create(OneMessagePtr *message) {
    if (message == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    auto m = new Message();
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_ALLOCATION_FAILED;
    }

    *message = (OneMessagePtr)m;
    return ONE_ERROR_NONE;
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

Error message_init(OneMessagePtr message, int code, const char *data, unsigned int size) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    return m->init(static_cast<Opcode>(code), {data, size});
}

Error message_code(OneMessagePtr message, int *code) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (code == nullptr) {
        return ONE_ERROR_VALIDATION_CODE_IS_NULLPTR;
    }

    *code = static_cast<int>(m->code());
    return ONE_ERROR_NONE;
}

Error message_val_int(OneMessagePtr message, const char *key, int *val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().val_int(key, *val);
}

Error message_val_string(OneMessagePtr message, const char *key, char **val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    std::string value;
    Error err = m->payload().val_string(key, value);
    if (is_error(err)) {
        return err;
    }

    // Todo: the string copy.

    return ONE_ERROR_NONE;
}

Error message_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    auto a = (Array *)val;
    if (a == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().val_array(key, *a);
}

Error message_val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    auto o = (Object *)val;
    if (o == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().val_object(key, *o);
}

Error message_set_val_int(OneMessagePtr message, const char *key, int val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    return m->payload().set_val_int(key, val);
}

Error message_set_val_string(OneMessagePtr message, const char *key, const char *val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().set_val_string(key, val);
}

Error message_set_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    auto a = (Array *)val;
    if (a == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().set_val_array(key, *a);
}

Error message_set_val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    auto o = (Object *)val;
    if (o == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().set_val_object(key, *o);
}

Error message_prepare_error_response(OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    return messages::prepare_error_response(*m);
}

Error message_prepare_live_state_response(int player, int max_player, const char *name,
                                          const char *map, const char *mode, const char *version,
                                          OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    if (name == nullptr) {
        return ONE_ERROR_VALIDATION_NAME_IS_NULLPTR;
    }

    if (map == nullptr) {
        return ONE_ERROR_VALIDATION_MAP_IS_NULLPTR;
    }

    if (mode == nullptr) {
        return ONE_ERROR_VALIDATION_MODE_IS_NULLPTR;
    }

    if (version == nullptr) {
        return ONE_ERROR_VALIDATION_VERSION_IS_NULLPTR;
    }

    return messages::prepare_live_state_response(player, max_player, name, map, mode, version, *m);
}

Error message_prepare_host_information_request(OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    return messages::prepare_host_information_request(*m);
}

Error server_create(size_t max_message_in, size_t max_message_out, OneServerPtr *server) {
    if (server == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    auto s = new Server();
    if (s == nullptr) {
        return ONE_ERROR_SERVER_ALLOCATION_FAILED;
    }

    auto err = s->init(max_message_in, max_message_out);
    if (is_error(err)) {
        delete s;
        return err;
    }

    *server = (OneServerPtr)s;
    return ONE_ERROR_NONE;
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

Error server_update(OneServerPtr server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    return s->update();
}

int server_status(OneServerPtr const server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    return static_cast<int>(s->status());
}

Error server_listen(OneServerPtr server, unsigned int port, int queueLength) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    return s->listen(port, queueLength);
}

Error server_shutdown(OneServerPtr server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    return s->shutdown();
}

Error server_send_error_response(OneServerPtr server, OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    return s->send_error_response(*m);
}

Error server_send_live_state_response(OneServerPtr server, OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    return s->send_live_state_response(*m);
}

Error server_send_host_information_request(OneServerPtr server, OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_IS_NULLPTR;
    }

    return s->send_host_information_request(*m);
}

Error server_set_soft_stop_callback(OneServerPtr server, void (*callback)(void *, int),
                                    void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_soft_stop_callback(callback, data);
    return ONE_ERROR_NONE;
}

Error server_set_allocated_callback(OneServerPtr server, void (*callback)(void *, void *),
                                    void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_allocated_callback(callback, data);
    return ONE_ERROR_NONE;
}

Error server_set_meta_data_callback(OneServerPtr server, void (*callback)(void *, void *),
                                    void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_meta_data_callback(callback, data);
    return ONE_ERROR_NONE;
}

Error server_set_live_state_request_callback(OneServerPtr server, void (*callback)(void *),
                                             void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_live_state_request_callback(callback, data);
    return ONE_ERROR_NONE;
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

OneError one_message_create(OneMessagePtr *message) {
    return one::message_create(message);
}

void one_message_destroy(OneMessagePtr *message) {
    one::message_destroy(message);
}

OneError one_message_init(OneMessagePtr message, int code, const char *data, unsigned int size) {
    return one::message_init(message, code, data, size);
}

OneError one_message_code(OneMessagePtr message, int *code) {
    return one::message_code(message, code);
}

OneError one_message_val_int(OneMessagePtr message, const char *key, int *val) {
    return one::message_val_int(message, key, val);
}

OneError one_message_val_string(OneMessagePtr message, const char *key, char **val) {
    return one::message_val_string(message, key, val);
}

OneError one_message_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    return one::message_val_array(message, key, val);
}

OneError one_message_val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
    return one::message_val_object(message, key, val);
}

OneError one_message_set_val_int(OneMessagePtr message, const char *key, int val) {
    return one::message_set_val_int(message, key, val);
}

OneError one_message_set_val_string(OneMessagePtr message, const char *key, const char *val) {
    return one::message_set_val_string(message, key, val);
}

OneError one_message_set_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    return one::message_set_val_array(message, key, val);
}

OneError one_message_set_val_object(OneMessagePtr message, const char *key, OneObjectPtr val) {
    return one::message_set_val_object(message, key, val);
}

OneError one_message_prepare_error_response(OneMessagePtr message) {
    return one::message_prepare_error_response(message);
}

OneError one_message_prepare_live_state_response(int player, int max_player, const char *name,
                                                 const char *map, const char *mode,
                                                 const char *version, OneMessagePtr message) {
    return one::message_prepare_live_state_response(player, max_player, name, map, mode, version,
                                                    message);
}

OneError one_message_prepare_host_information_request(OneMessagePtr message) {
    return one::message_prepare_host_information_request(message);
}

OneError one_server_create(size_t max_message_in, size_t max_message_out, OneServerPtr *server) {
    return one::server_create(max_message_in, max_message_out, server);
}

void one_server_destroy(OneServerPtr *server) {
    return one::server_destroy(server);
}

OneError one_server_update(OneServerPtr server) {
    return one::server_update(server);
}

int one_server_status(OneServerPtr const server) {
    return one::server_status(server);
}

OneError one_server_listen(OneServerPtr server, unsigned int port, int queueLength) {
    return one::server_listen(server, port, queueLength);
}

OneError one_server_shutdown(OneServerPtr server) {
    return one::server_shutdown(server);
}

OneError one_server_send_error_response(OneServerPtr server, OneMessagePtr message) {
    return one::server_send_error_response(server, message);
}

OneError one_server_send_live_state_response(OneServerPtr server, OneMessagePtr message) {
    return one::server_send_live_state_response(server, message);
}

OneError one_server_send_host_information_request(OneServerPtr server, OneMessagePtr message) {
    return one::server_send_host_information_request(server, message);
}

OneError one_server_set_soft_stop_callback(OneServerPtr server,
                                           void (*callback)(void *data, int timeout), void *data) {
    return one::server_set_soft_stop_callback(server, callback, data);
}

OneError one_server_set_allocated_callback(OneServerPtr server,
                                           void (*callback)(void *data, void *array), void *data) {
    return one::server_set_allocated_callback(server, callback, data);
}

OneError one_server_set_meta_data_callback(OneServerPtr server,
                                           void (*callback)(void *data, void *array), void *data) {
    return one::server_set_meta_data_callback(server, callback, data);
}

OneError one_server_set_live_state_request_callback(OneServerPtr server,
                                                    void (*callback)(void *data), void *data) {
    return one::server_set_live_state_request_callback(server, callback, data);
}

void one_allocator_set_alloc(void *(callback)(unsigned int size)) {
    one::allocator_set_alloc(callback);
}

void one_allocator_set_free(void(callback)(void *)) {
    one::allocator_set_free(callback);
}

};  // extern "C"
