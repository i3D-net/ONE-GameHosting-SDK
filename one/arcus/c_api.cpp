#include <one/arcus/c_api.h>

#include <one/arcus/allocator.h>
#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/arcus/opcode.h>
#include <one/arcus/server.h>
#include <one/arcus/types.h>

#include <utility>
#include <cstring>

namespace i3d {
namespace one {
namespace {

OneError array_create(OneArrayPtr *array) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    auto a = allocator::create<Array>();
    if (a == nullptr) {
        return ONE_ERROR_ARRAY_ALLOCATION_FAILED;
    }

    *array = (OneArrayPtr)a;
    return ONE_ERROR_NONE;
}

void array_destroy(OneArrayPtr array) {
    if (array == nullptr) {
        return;
    }

    auto a = (Array *)(array);
    allocator::destroy<Array>(a);
}

OneError array_copy(OneArrayPtr source, OneArrayPtr destination) {
    if (source == nullptr) {
        return ONE_ERROR_VALIDATION_SOURCE_IS_NULLPTR;
    }

    if (destination == nullptr) {
        return ONE_ERROR_VALIDATION_DESTINATION_IS_NULLPTR;
    }

    auto s = (Array *)source;
    auto d = (Array *)destination;
    *d = *s;
    return ONE_ERROR_NONE;
}

OneError array_clear(OneArrayPtr array) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    auto a = (Array *)array;
    a->clear();
    return ONE_ERROR_NONE;
}

OneError array_reserve(OneArrayPtr array, size_t size) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    auto a = (Array *)array;
    a->reserve(size);
    return ONE_ERROR_NONE;
}

OneError array_is_empty(OneArrayPtr array, bool *empty) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (empty == nullptr) {
        return ONE_ERROR_VALIDATION_EMPTY_IS_NULLPTR;
    }

    auto a = (Array *)array;
    *empty = a->is_empty();
    return ONE_ERROR_NONE;
}

OneError array_size(OneArrayPtr array, int *size) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (size == nullptr) {
        return ONE_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    auto a = (Array *)array;
    *size = a->size();
    return ONE_ERROR_NONE;
}

OneError array_capacity(OneArrayPtr array, int *capacity) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (capacity == nullptr) {
        return ONE_ERROR_VALIDATION_CAPACITY_IS_NULLPTR;
    }

    auto a = (Array *)array;
    *capacity = static_cast<int>(a->capacity());
    return ONE_ERROR_NONE;
}

OneError array_push_back_bool(OneArrayPtr array, bool val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    auto a = (Array *)array;
    a->push_back_bool(val);
    return ONE_ERROR_NONE;
}

OneError array_push_back_int(OneArrayPtr array, int val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    auto a = (Array *)array;
    a->push_back_int(val);
    return ONE_ERROR_NONE;
}

OneError array_push_back_string(OneArrayPtr array, const char *val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)array;
    a->push_back_string(String(val));
    return ONE_ERROR_NONE;
}

OneError array_push_back_array(OneArrayPtr array, OneArrayPtr val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)array;
    auto v = (Array *)val;
    a->push_back_array(*v);
    return ONE_ERROR_NONE;
}

OneError array_push_back_object(OneArrayPtr array, OneObjectPtr val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)array;
    auto v = (Object *)val;
    a->push_back_object(*v);
    return ONE_ERROR_NONE;
}

OneError array_pop_back(OneArrayPtr array) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    auto a = (Array *)array;
    a->pop_back();
    return ONE_ERROR_NONE;
}

OneError array_is_val_bool(OneArrayPtr array, unsigned int pos, bool *result) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    *result = a->is_val_bool(pos);
    return ONE_ERROR_NONE;
}

OneError array_is_val_int(OneArrayPtr array, unsigned int pos, bool *result) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }
    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    *result = a->is_val_int(pos);
    return ONE_ERROR_NONE;
}

OneError array_is_val_string(OneArrayPtr array, unsigned int pos, bool *result) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    *result = a->is_val_string(pos);
    return ONE_ERROR_NONE;
}

OneError array_is_val_array(OneArrayPtr array, unsigned int pos, bool *result) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    *result = a->is_val_array(pos);
    return ONE_ERROR_NONE;
}

OneError array_is_val_object(OneArrayPtr array, unsigned int pos, bool *result) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    *result = a->is_val_object(pos);
    return ONE_ERROR_NONE;
}

OneError array_val_bool(OneArrayPtr array, unsigned int pos, bool *val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    return a->val_bool(pos, *val);
}

OneError array_val_int(OneArrayPtr array, unsigned int pos, int *val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    return a->val_int(pos, *val);
}

OneError array_val_string_size(OneArrayPtr array, unsigned int pos, int *size) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (size == nullptr) {
        return ONE_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    size_t result = 0;
    auto err = a->val_string_size(pos, result);
    if (is_error(err)) {
        return err;
    }

    *size = static_cast<int>(result);
    return ONE_ERROR_NONE;
}

OneError array_val_string(OneArrayPtr array, unsigned int pos, char *val, size_t size) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)(array);

    size_t val_size = 0;
    auto err = a->val_string_size(pos, val_size);
    if (is_error(err)) {
        return err;
    }

    if (size < val_size) {
        return ONE_ERROR_VALIDATION_VAL_SIZE_IS_TOO_SMALL;
    }

    String s(val);
    err = a->val_string(pos, s);
    if (is_error(err)) {
        return err;
    }

    std::strncpy(val, s.c_str(), s.size());
    return ONE_ERROR_NONE;
}

OneError array_val_array(OneArrayPtr array, unsigned int pos, OneArrayPtr val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    auto v = (Array *)(val);
    return a->val_array(pos, *v);
}

OneError array_val_object(OneArrayPtr array, unsigned int pos, OneObjectPtr val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    auto v = (Object *)(val);
    return a->val_object(pos, *v);
}

OneError array_set_val_bool(OneArrayPtr array, unsigned int pos, bool val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    return a->set_val_bool(pos, val);
}

OneError array_set_val_int(OneArrayPtr array, unsigned int pos, int val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    return a->set_val_int(pos, val);
}

OneError array_set_val_string(OneArrayPtr array, unsigned int pos, const char *val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    return a->set_val_string(pos, val);
}

OneError array_set_val_array(OneArrayPtr array, unsigned int pos, OneArrayPtr val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    auto v = (Array *)(val);
    return a->set_val_array(pos, *v);
}

OneError array_set_val_object(OneArrayPtr array, unsigned int pos, OneObjectPtr val) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto a = (Array *)(array);
    auto v = (Object *)(val);
    return a->set_val_object(pos, *v);
}

OneError object_create(OneObjectPtr *object) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    auto o = allocator::create<Object>();
    if (o == nullptr) {
        return ONE_ERROR_OBJECT_ALLOCATION_FAILED;
    }

    *object = (OneObjectPtr)o;
    return ONE_ERROR_NONE;
}

void object_destroy(OneObjectPtr object) {
    if (object == nullptr) {
        return;
    }

    auto o = reinterpret_cast<Object *>(object);
    allocator::destroy<Object>(o);
}

OneError object_is_val_bool(OneObjectPtr object, const char *key, bool *result) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto o = (Object *)object;
    *result = o->is_val_bool(key);
    return ONE_ERROR_NONE;
}

OneError object_is_val_int(OneObjectPtr object, const char *key, bool *result) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto o = (Object *)object;
    *result = o->is_val_int(key);
    return ONE_ERROR_NONE;
}

OneError object_is_val_string(OneObjectPtr object, const char *key, bool *result) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto o = (Object *)object;
    *result = o->is_val_string(key);
    return ONE_ERROR_NONE;
}

OneError object_is_val_array(OneObjectPtr object, const char *key, bool *result) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto o = (Object *)object;
    *result = o->is_val_array(key);
    return ONE_ERROR_NONE;
}

OneError object_is_val_object(OneObjectPtr object, const char *key, bool *result) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    auto o = (Object *)object;
    *result = o->is_val_object(key);
    return ONE_ERROR_NONE;
}

OneError object_val_bool(OneObjectPtr object, const char *key, bool *val) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto o = (Object *)object;
    return o->val_bool(key, *val);
}

OneError object_val_int(OneObjectPtr object, const char *key, int *val) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto o = (Object *)object;
    return o->val_int(key, *val);
}

OneError object_val_string_size(OneObjectPtr object, const char *key, int *size) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (size == nullptr) {
        return ONE_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    auto o = (Object *)object;
    size_t result = 0;
    auto err = o->val_string_size(key, result);
    if (is_error(err)) {
        return err;
    }

    *size = static_cast<int>(result);
    return ONE_ERROR_NONE;
}

OneError object_val_string(OneObjectPtr object, const char *key, char *val, size_t size) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto o = (Object *)object;
    size_t val_size = 0;
    auto err = o->val_string_size(key, val_size);
    if (is_error(err)) {
        return err;
    }

    if (size < val_size) {
        return ONE_ERROR_VALIDATION_VAL_SIZE_IS_TOO_SMALL;
    }

    String s;
    err = o->val_string(key, s);
    if (is_error(err)) {
        return err;
    }

    std::strncpy(val, s.c_str(), s.size());
    return ONE_ERROR_NONE;
}

OneError object_val_array(OneObjectPtr object, const char *key, OneArrayPtr val) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto o = (Object *)object;
    auto v = (Array *)val;
    return o->val_array(key, *v);
}

OneError object_val_object(OneObjectPtr object, const char *key, OneObjectPtr val) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto o = (Object *)object;
    auto v = (Object *)val;
    return o->val_object(key, *v);
}

OneError object_set_val_bool(OneObjectPtr object, const char *key, bool val) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    auto o = (Object *)object;
    return o->set_val_bool(key, val);
}

OneError object_set_val_int(OneObjectPtr object, const char *key, int val) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    auto o = (Object *)object;
    return o->set_val_int(key, val);
}

OneError object_set_val_string(OneObjectPtr object, const char *key, const char *val) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto o = (Object *)object;
    return o->set_val_string(key, String(val));
}

OneError object_set_val_array(OneObjectPtr object, const char *key, OneArrayPtr val) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto o = (Object *)object;
    auto v = (Array *)val;
    return o->set_val_array(key, *v);
}

OneError object_set_val_object(OneObjectPtr object, const char *key, OneObjectPtr val) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    auto o = (Object *)object;
    auto v = (Object *)val;
    return o->set_val_object(key, *v);
}

Error server_create(OneServerPtr *server) {
    if (server == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    auto s = allocator::create<Server>();
    if (s == nullptr) {
        return ONE_ERROR_SERVER_ALLOCATION_FAILED;
    }

    auto err = s->init();
    if (is_error(err)) {
        allocator::destroy<Server>(s);
        return err;
    }

    *server = (OneServerPtr)s;
    return ONE_ERROR_NONE;
}

Error server_set_logger(OneServerPtr server, OneLogFn logFn, void *userdata) {
    if (server == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    auto logFnLambda = [logFn](void *userdata, LogLevel level, const String &message) {
        logFn(userdata, static_cast<OneLogLevel>(level), message.c_str());
    };
    Logger logger(logFnLambda, userdata);

    auto s = (Server *)(server);
    s->set_logger(logger);
    return ONE_ERROR_NONE;
}

void server_destroy(OneServerPtr server) {
    if (server == nullptr) {
        return;
    }

    auto s = (Server *)(server);
    allocator::destroy<Server>(s);
}

Error server_update(OneServerPtr server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    return s->update();
}

Error server_status(OneServerPtr const server, OneServerStatus *status) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (status == nullptr) {
        return ONE_ERROR_VALIDATION_STATUS_IS_NULLPTR;
    }

    *status = static_cast<OneServerStatus>(s->status());
    return ONE_ERROR_NONE;
}

Error server_listen(OneServerPtr server, unsigned int port) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    return s->listen(port);
}

Error server_shutdown(OneServerPtr server) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    return s->shutdown();
}

Error server_set_live_state(OneServerPtr server, int players, int max_players,
                            const char *name, const char *map, const char *mode,
                            const char *version, OneObjectPtr additional_data) {
    auto s = reinterpret_cast<Server *>(server);
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
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

    Object *object = nullptr;
    if (additional_data != nullptr) {
        object = reinterpret_cast<Object *>(additional_data);
    }

    return s->set_live_state(players, max_players, name, map, mode, version, object);
}

Error server_set_application_instance_status(OneServerPtr server,
                                             OneApplicationInstanceStatus status) {
    auto s = reinterpret_cast<Server *>(server);
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    return s->set_application_instance_status(
        static_cast<Server::ApplicationInstanceStatus>(status));
}

Error server_set_soft_stop_callback(OneServerPtr server, void (*callback)(void *, int),
                                    void *userdata) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_soft_stop_callback(callback, userdata);
    return ONE_ERROR_NONE;
}

Error server_set_allocated_callback(OneServerPtr server, void (*callback)(void *, void *),
                                    void *userdata) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_allocated_callback(callback, userdata);
    return ONE_ERROR_NONE;
}

Error server_set_metadata_callback(OneServerPtr server, void (*callback)(void *, void *),
                                   void *userdata) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_metadata_callback(callback, userdata);
    return ONE_ERROR_NONE;
}

Error server_set_host_information_callback(OneServerPtr server,
                                           void (*callback)(void *, void *), void *userdata) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_host_information_callback(callback, userdata);
    return ONE_ERROR_NONE;
}

Error server_set_application_instance_information_callback(
    OneServerPtr server, void (*callback)(void *, void *), void *userdata) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_application_instance_information_callback(callback, userdata);
    return ONE_ERROR_NONE;
}

void allocator_set_alloc(void *(*callback)(unsigned int size)) {
    // Wrapper for size_t.
    auto wrapper = [callback](size_t size) -> void * {
        return callback(static_cast<unsigned int>(size));
    };
    allocator::set_alloc(wrapper);
}

void allocator_set_free(void(callback)(void *)) {
    allocator::set_free(callback);
}

void allocator_set_realloc(void *(*callback)(void *, unsigned int size)) {
    // Wrapper for size_t.
    auto wrapper = [callback](void *p, size_t size) -> void * {
        return callback(p, static_cast<unsigned int>(size));
    };
    allocator::set_realloc(wrapper);
}

}  // Unnamed namespace.
}  // namespace one
}  // namespace i3d

namespace one = i3d::one;

extern "C" {

OneError one_array_create(OneArrayPtr *array) {
    return one::array_create(array);
}

void one_array_destroy(OneArrayPtr array) {
    return one::array_destroy(array);
}

OneError one_array_copy(OneArrayPtr source, OneArrayPtr destination) {
    return one::array_copy(source, destination);
}

OneError one_array_clear(OneArrayPtr array) {
    return one::array_clear(array);
}

OneError one_array_reserve(OneArrayPtr array, int size) {
    return one::array_reserve(array, size);
}

OneError one_array_is_empty(OneArrayPtr array, bool *empty) {
    return one::array_is_empty(array, empty);
}

OneError one_array_size(OneArrayPtr array, int *size) {
    return one::array_size(array, size);
}

OneError one_array_capacity(OneArrayPtr array, int *capacity) {
    return one::array_capacity(array, capacity);
}

OneError one_array_push_back_bool(OneArrayPtr array, bool val) {
    return one::array_push_back_bool(array, val);
}

OneError one_array_push_back_int(OneArrayPtr array, int val) {
    return one::array_push_back_int(array, val);
}

OneError one_array_push_back_string(OneArrayPtr array, const char *val) {
    return one::array_push_back_string(array, val);
}

OneError one_array_push_back_array(OneArrayPtr array, OneArrayPtr val) {
    return one::array_push_back_array(array, val);
}

OneError one_array_push_back_object(OneArrayPtr array, OneObjectPtr val) {
    return one::array_push_back_object(array, val);
}

OneError one_array_pop_back(OneArrayPtr array) {
    return one::array_pop_back(array);
}

OneError one_array_is_val_bool(OneArrayPtr array, unsigned int pos, bool *result) {
    return one::array_is_val_bool(array, pos, result);
}

OneError one_array_is_val_int(OneArrayPtr array, unsigned int pos, bool *result) {
    return one::array_is_val_int(array, pos, result);
}

OneError one_array_is_val_string(OneArrayPtr array, unsigned int pos, bool *result) {
    return one::array_is_val_string(array, pos, result);
}

OneError one_array_is_val_array(OneArrayPtr array, unsigned int pos, bool *result) {
    return one::array_is_val_array(array, pos, result);
}

OneError one_array_is_val_object(OneArrayPtr array, unsigned int pos, bool *result) {
    return one::array_is_val_object(array, pos, result);
}

OneError one_array_val_bool(OneArrayPtr array, unsigned int pos, bool *val) {
    return one::array_val_bool(array, pos, val);
}

OneError one_array_val_int(OneArrayPtr array, unsigned int pos, int *val) {
    return one::array_val_int(array, pos, val);
}

OneError one_array_val_string_size(OneArrayPtr array, unsigned int pos, int *size) {
    return one::array_val_string_size(array, pos, size);
}

OneError one_array_val_string(OneArrayPtr array, unsigned int pos, char *val, int size) {
    return one::array_val_string(array, pos, val, size);
}

OneError one_array_val_array(OneArrayPtr array, unsigned int pos, OneArrayPtr val) {
    return one::array_val_array(array, pos, val);
}

OneError one_array_val_object(OneArrayPtr array, unsigned int pos, OneObjectPtr val) {
    return one::array_val_object(array, pos, val);
}

OneError one_array_set_val_bool(OneArrayPtr array, unsigned int pos, bool val) {
    return one::array_set_val_bool(array, pos, val);
}

OneError one_array_set_val_int(OneArrayPtr array, unsigned int pos, int val) {
    return one::array_set_val_int(array, pos, val);
}

OneError one_array_set_val_string(OneArrayPtr array, unsigned int pos, const char *val) {
    return one::array_set_val_string(array, pos, val);
}

OneError one_array_set_val_array(OneArrayPtr array, unsigned int pos, OneArrayPtr val) {
    return one::array_set_val_array(array, pos, val);
}

OneError one_array_set_val_object(OneArrayPtr array, unsigned int pos, OneObjectPtr val) {
    return one::array_set_val_object(array, pos, val);
}

OneError one_object_create(OneObjectPtr *object) {
    return one::object_create(object);
}

void one_object_destroy(OneObjectPtr object) {
    one::object_destroy(object);
}

OneError one_object_is_val_bool(OneObjectPtr object, const char *key, bool *result) {
    return one::object_is_val_bool(object, key, result);
}

OneError one_object_is_val_int(OneObjectPtr object, const char *key, bool *result) {
    return one::object_is_val_int(object, key, result);
}

OneError one_object_is_val_string(OneObjectPtr object, const char *key, bool *result) {
    return one::object_is_val_string(object, key, result);
}

OneError one_object_is_val_array(OneObjectPtr object, const char *key, bool *result) {
    return one::object_is_val_array(object, key, result);
}

OneError one_object_is_val_object(OneObjectPtr object, const char *key, bool *result) {
    return one::object_is_val_object(object, key, result);
}

OneError one_object_val_bool(OneObjectPtr object, const char *key, bool *val) {
    return one::object_val_bool(object, key, val);
}

OneError one_object_val_int(OneObjectPtr object, const char *key, int *val) {
    return one::object_val_int(object, key, val);
}

OneError one_object_val_string_size(OneObjectPtr object, const char *key, int *size) {
    return one::object_val_string_size(object, key, size);
}

OneError one_object_val_string(OneObjectPtr object, const char *key, char *val,
                               int size) {
    return one::object_val_string(object, key, val, size);
}

OneError one_object_val_array(OneObjectPtr object, const char *key, OneArrayPtr val) {
    return one::object_val_array(object, key, val);
}

OneError one_object_val_object(OneObjectPtr object, const char *key, OneObjectPtr val) {
    return one::object_val_object(object, key, val);
}

OneError one_object_set_val_bool(OneObjectPtr object, const char *key, bool val) {
    return one::object_set_val_bool(object, key, val);
}

OneError one_object_set_val_int(OneObjectPtr object, const char *key, int val) {
    return one::object_set_val_int(object, key, val);
}

OneError one_object_set_val_string(OneObjectPtr object, const char *key,
                                   const char *val) {
    return one::object_set_val_string(object, key, val);
}

OneError one_object_set_val_array(OneObjectPtr object, const char *key, OneArrayPtr val) {
    return one::object_set_val_array(object, key, val);
}

OneError one_object_set_val_object(OneObjectPtr object, const char *key,
                                   OneObjectPtr val) {
    return one::object_set_val_object(object, key, val);
}

OneError one_server_create(OneServerPtr *server) {
    return one::server_create(server);
}

OneError one_server_set_logger(OneServerPtr server, OneLogFn logFn, void *userdata) {
    return one::server_set_logger(server, logFn, userdata);
}

void one_server_destroy(OneServerPtr server) {
    return one::server_destroy(server);
}

OneError one_server_update(OneServerPtr server) {
    return one::server_update(server);
}

OneError one_server_status(OneServerPtr const server, OneServerStatus *status) {
    return one::server_status(server, status);
}

OneError one_server_listen(OneServerPtr server, unsigned int port) {
    return one::server_listen(server, port);
}

OneError one_server_shutdown(OneServerPtr server) {
    return one::server_shutdown(server);
}

OneError one_server_set_live_state(OneServerPtr server, int players, int max_players,
                                   const char *name, const char *map, const char *mode,
                                   const char *version, OneObjectPtr additional_data) {
    return one::server_set_live_state(server, players, max_players, name, map, mode,
                                      version, additional_data);
}

OneError one_server_set_application_instance_status(OneServerPtr server,
                                                    OneApplicationInstanceStatus status) {
    return one::server_set_application_instance_status(server, status);
}

OneError one_server_set_soft_stop_callback(OneServerPtr server,
                                           void (*callback)(void *userdata, int timeout),
                                           void *userdata) {
    return one::server_set_soft_stop_callback(server, callback, userdata);
}

OneError one_server_set_allocated_callback(OneServerPtr server,
                                           void (*callback)(void *userdata, void *array),
                                           void *userdata) {
    return one::server_set_allocated_callback(server, callback, userdata);
}

OneError one_server_set_metadata_callback(OneServerPtr server,
                                          void (*callback)(void *userdata, void *array),
                                          void *userdata) {
    return one::server_set_metadata_callback(server, callback, userdata);
}

OneError one_server_set_host_information_callback(OneServerPtr server,
                                                  void (*callback)(void *, void *),
                                                  void *userdata) {
    return one::server_set_host_information_callback(server, callback, userdata);
}

OneError one_server_set_application_instance_information_callback(
    OneServerPtr server, void (*callback)(void *, void *), void *userdata) {
    return one::server_set_application_instance_information_callback(server, callback,
                                                                     userdata);
}

void one_allocator_set_alloc(void *(*callback)(unsigned int size)) {
    one::allocator_set_alloc(callback);
}

void one_allocator_set_free(void (*callback)(void *)) {
    one::allocator_set_free(callback);
}

void one_allocator_set_realloc(void *(*callback)(void *, unsigned int size)) {
    one::allocator_set_realloc(callback);
}

};  // extern "C"
