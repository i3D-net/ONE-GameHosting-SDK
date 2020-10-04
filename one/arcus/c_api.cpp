#include <one/arcus/c_api.h>

#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/arcus/opcode.h>
#include <one/arcus/server.h>

#include <utility>
#include <string>
#include <cstring>

namespace i3d {
namespace one {
namespace {

Error message_create(OneMessagePtr *message) {
    if (message == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    auto m = new Message();
    if (m == nullptr) {
        return ONE_ERROR_MESSAGE_ALLOCATION_FAILED;
    }

    *message = (OneMessagePtr)m;
    return ONE_ERROR_NONE;
}

void message_destroy(OneMessagePtr message) {
    if (message == nullptr) {
        return;
    }

    auto m = (Message *)(message);
    delete m;
}

OneError message_reset(OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    m->reset();
    return ONE_ERROR_NONE;
}

Error message_code(OneMessagePtr message, int *code) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (code == nullptr) {
        return ONE_ERROR_VALIDATION_CODE_IS_NULLPTR;
    }

    *code = static_cast<int>(m->code());
    return ONE_ERROR_NONE;
}

Error message_set_code(OneMessagePtr message, int code) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    return m->init(static_cast<Opcode>(code), Payload());
}

OneError message_is_val_bool(OneMessagePtr message, const char *key, bool *result) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    *result = m->payload().is_val_bool(key);
    return ONE_ERROR_NONE;
}

OneError message_is_val_int(OneMessagePtr message, const char *key, bool *result) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    *result = m->payload().is_val_int(key);
    return ONE_ERROR_NONE;
}

OneError message_is_val_string(OneMessagePtr message, const char *key, bool *result) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    *result = m->payload().is_val_string(key);
    return ONE_ERROR_NONE;
}

OneError message_is_val_array(OneMessagePtr message, const char *key, bool *result) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    *result = m->payload().is_val_array(key);
    return ONE_ERROR_NONE;
}

OneError message_is_val_object(OneMessagePtr message, const char *key, bool *result) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (result == nullptr) {
        return ONE_ERROR_VALIDATION_RESULT_IS_NULLPTR;
    }

    *result = m->payload().is_val_object(key);
    return ONE_ERROR_NONE;
}

Error message_val_bool(OneMessagePtr message, const char *key, bool *val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().val_bool(key, *val);
}

Error message_val_int(OneMessagePtr message, const char *key, int *val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (val == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().val_int(key, *val);
}

Error message_val_string_size(OneMessagePtr message, const char *key, int *size) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    if (size == nullptr) {
        return ONE_ERROR_VALIDATION_SIZE_IS_NULLPTR;
    }

    std::string value;
    Error err = m->payload().val_string(key, value);
    if (is_error(err)) {
        return err;
    }

    *size = value.size();
    return ONE_ERROR_NONE;
}

Error message_val_string(OneMessagePtr message, const char *key, char *val,
                         int val_size) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
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

    if (val_size < value.size()) {
        return ONE_ERROR_VALIDATION_VAL_SIZE_IS_TOO_SMALL;
    }

    std::strncpy(val, value.c_str(), value.size());
    return ONE_ERROR_NONE;
}

Error message_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
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
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
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

Error message_val_root_object(OneMessagePtr message, OneObjectPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    auto o = (Object *)val;
    if (o == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().val_root_object(*o);
}

Error message_set_val_bool(OneMessagePtr message, const char *key, bool val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    return m->payload().set_val_bool(key, val);
}

Error message_set_val_int(OneMessagePtr message, const char *key, int val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    return m->payload().set_val_int(key, val);
}

Error message_set_val_string(OneMessagePtr message, const char *key, const char *val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
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
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
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
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
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

Error message_set_val_root_object(OneMessagePtr message, OneObjectPtr val) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    auto o = (Object *)val;
    if (o == nullptr) {
        return ONE_ERROR_VALIDATION_VAL_IS_NULLPTR;
    }

    return m->payload().set_val_root_object(*o);
}

OneError array_create(OneArrayPtr *array) {
    if (array == nullptr) {
        return ONE_ERROR_VALIDATION_ARRAY_IS_NULLPTR;
    }

    auto a = new Array();
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
    delete a;
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
    a->push_back_string(std::string(val));
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

    std::string s(val);
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

    auto o = new Object();
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
    delete o;
}

OneError object_copy(OneObjectPtr source, OneObjectPtr destination) {
    if (source == nullptr) {
        return ONE_ERROR_VALIDATION_SOURCE_IS_NULLPTR;
    }

    if (destination == nullptr) {
        return ONE_ERROR_VALIDATION_DESTINATION_IS_NULLPTR;
    }

    auto s = (Object *)source;
    auto d = (Object *)destination;
    *d = *s;
    return ONE_ERROR_NONE;
}

OneError object_clear(OneObjectPtr object) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    auto o = (Object *)object;
    o->clear();
    return ONE_ERROR_NONE;
}

OneError object_is_empty(OneObjectPtr object, bool *empty) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (empty == nullptr) {
        return ONE_ERROR_VALIDATION_EMPTY_IS_NULLPTR;
    }

    auto o = (Object *)object;
    *empty = o->is_empty();
    return ONE_ERROR_NONE;
}

OneError object_remove_key(OneObjectPtr object, const char *key) {
    if (object == nullptr) {
        return ONE_ERROR_VALIDATION_OBJECT_IS_NULLPTR;
    }

    if (key == nullptr) {
        return ONE_ERROR_VALIDATION_KEY_IS_NULLPTR;
    }

    auto o = (Object *)object;
    return o->remove_key(key);
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

    std::string s;
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
    return o->set_val_string(key, std::string(val));
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

Error message_prepare_live_state_response(int players, int max_players, const char *name,
                                          const char *map, const char *mode,
                                          const char *version, OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
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

    return messages::prepare_live_state_response(players, max_players, name, map, mode,
                                                 version, *m);
}

Error message_prepare_application_instance_information_request(OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    return messages::prepare_application_instance_information_request(*m);
}

Error message_prepare_application_instance_get_status_request(OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    return messages::prepare_application_instance_get_status_request(*m);
}

Error message_prepare_application_instance_set_status_request(int status,
                                                              OneMessagePtr message) {
    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    return messages::prepare_application_instance_set_status_request(status, *m);
}

Error server_create(OneServerPtr *server) {
    if (server == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    auto s = new Server();
    if (s == nullptr) {
        return ONE_ERROR_SERVER_ALLOCATION_FAILED;
    }

    auto err = s->init();
    if (is_error(err)) {
        delete s;
        return err;
    }

    *server = (OneServerPtr)s;
    return ONE_ERROR_NONE;
}

void server_destroy(OneServerPtr server) {
    if (server == nullptr) {
        return;
    }

    auto s = (Server *)(server);
    delete s;
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

Error server_send_live_state_response(OneServerPtr server, OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    return s->send_live_state_response(*m);
}

Error server_send_application_instance_information_request(OneServerPtr server,
                                                           OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    return s->send_application_instance_information_request(*m);
}

Error server_send_application_instance_get_status_request(OneServerPtr server,
                                                          OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    return s->send_application_instance_get_status_request(*m);
}

Error server_send_application_instance_set_status_request(OneServerPtr server,
                                                          OneMessagePtr message) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    auto m = (Message *)message;
    if (m == nullptr) {
        return ONE_ERROR_VALIDATION_MESSAGE_IS_NULLPTR;
    }

    return s->send_application_instance_set_status_request(*m);
}

Error server_set_soft_stop_callback(OneServerPtr server, void (*callback)(void *, int),
                                    void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
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
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
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
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_meta_data_callback(callback, data);
    return ONE_ERROR_NONE;
}

Error server_set_host_information_response_callback(OneServerPtr server,
                                                    void (*callback)(void *, void *),
                                                    void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_host_information_response_callback(callback, data);
    return ONE_ERROR_NONE;
}

Error server_set_application_instance_information_response_callback(
    OneServerPtr server, void (*callback)(void *, void *), void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_application_instance_information_response_callback(callback, data);
    return ONE_ERROR_NONE;
}

Error server_set_application_instance_get_status_response_callback(
    OneServerPtr server, void (*callback)(void *, int), void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_application_instance_get_status_response_callback(callback, data);
    return ONE_ERROR_NONE;
}

Error server_set_application_instance_set_status_response_callback(
    OneServerPtr server, void (*callback)(void *, int), void *data) {
    auto s = (Server *)server;
    if (s == nullptr) {
        return ONE_ERROR_VALIDATION_SERVER_IS_NULLPTR;
    }

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    s->set_application_instance_set_status_response_callback(callback, data);
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
}  // namespace i3d

namespace one = i3d::one;

extern "C" {

OneError one_message_create(OneMessagePtr *message) {
    return one::message_create(message);
}

void one_message_destroy(OneMessagePtr message) {
    one::message_destroy(message);
}

OneError one_message_reset(OneMessagePtr message) {
    return one::message_reset(message);
}

OneError one_message_code(OneMessagePtr message, int *code) {
    return one::message_code(message, code);
}

OneError one_message_set_code(OneMessagePtr message, int code) {
    return one::message_set_code(message, code);
}

OneError one_message_is_val_bool(OneMessagePtr message, const char *key, bool *result) {
    return one::message_is_val_bool(message, key, result);
}
OneError one_message_is_val_int(OneMessagePtr message, const char *key, bool *result) {
    return one::message_is_val_int(message, key, result);
}
OneError one_message_is_val_string(OneMessagePtr message, const char *key, bool *result) {
    return one::message_is_val_string(message, key, result);
}
OneError one_message_is_val_array(OneMessagePtr message, const char *key, bool *result) {
    return one::message_is_val_array(message, key, result);
}
OneError one_message_is_val_object(OneMessagePtr message, const char *key, bool *result) {
    return one::message_is_val_object(message, key, result);
}

OneError one_message_val_bool(OneMessagePtr message, const char *key, bool *val) {
    return one::message_val_bool(message, key, val);
}

OneError one_message_val_int(OneMessagePtr message, const char *key, int *val) {
    return one::message_val_int(message, key, val);
}

OneError one_message_val_string_size(OneMessagePtr message, const char *key, int *size) {
    return one::message_val_string_size(message, key, size);
}

OneError one_message_val_string(OneMessagePtr message, const char *key, char *val,
                                int val_size) {
    return one::message_val_string(message, key, val, val_size);
}

OneError one_message_val_array(OneMessagePtr message, const char *key, OneArrayPtr val) {
    return one::message_val_array(message, key, val);
}

OneError one_message_val_object(OneMessagePtr message, const char *key,
                                OneObjectPtr val) {
    return one::message_val_object(message, key, val);
}

OneError one_message_val_root_object(OneMessagePtr message, OneObjectPtr val) {
    return one::message_val_root_object(message, val);
}

OneError one_message_set_val_bool(OneMessagePtr message, const char *key, bool val) {
    return one::message_set_val_bool(message, key, val);
}

OneError one_message_set_val_int(OneMessagePtr message, const char *key, int val) {
    return one::message_set_val_int(message, key, val);
}

OneError one_message_set_val_string(OneMessagePtr message, const char *key,
                                    const char *val) {
    return one::message_set_val_string(message, key, val);
}

OneError one_message_set_val_array(OneMessagePtr message, const char *key,
                                   OneArrayPtr val) {
    return one::message_set_val_array(message, key, val);
}

OneError one_message_set_val_object(OneMessagePtr message, const char *key,
                                    OneObjectPtr val) {
    return one::message_set_val_object(message, key, val);
}

OneError one_message_set_val_root_object(OneMessagePtr message, OneObjectPtr val) {
    return one::message_set_val_root_object(message, val);
}

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

OneError one_array_val_string(OneArrayPtr array, unsigned int pos, char *val,
                              int val_size) {
    return one::array_val_string(array, pos, val, val_size);
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

OneError one_object_copy(OneObjectPtr source, OneObjectPtr destination) {
    return one::object_copy(source, destination);
}

OneError one_object_clear(OneObjectPtr object) {
    return one::object_clear(object);
}

OneError one_object_is_empty(OneObjectPtr object, bool *empty) {
    return one::object_is_empty(object, empty);
}

OneError one_object_remove_key(OneObjectPtr object, const char *key) {
    return one::object_remove_key(object, key);
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

OneError one_message_prepare_live_state_response(int players, int max_players,
                                                 const char *name, const char *map,
                                                 const char *mode, const char *version,
                                                 OneMessagePtr message) {
    return one::message_prepare_live_state_response(players, max_players, name, map, mode,
                                                    version, message);
}

OneError one_message_prepare_application_instance_information_request(
    OneMessagePtr message) {
    return one::message_prepare_application_instance_information_request(message);
}

OneError one_message_prepare_application_instance_get_status_request(
    OneMessagePtr message) {
    return one::message_prepare_application_instance_get_status_request(message);
}

OneError one_message_prepare_application_instance_set_status_request(
    int status, OneMessagePtr message) {
    return one::message_prepare_application_instance_set_status_request(status, message);
}

OneError one_server_create(OneServerPtr *server) {
    return one::server_create(server);
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

OneError one_server_send_live_state_response(OneServerPtr server, OneMessagePtr message) {
    return one::server_send_live_state_response(server, message);
}

OneError one_server_send_application_instance_information_request(OneServerPtr server,
                                                                  OneMessagePtr message) {
    return one::server_send_application_instance_information_request(server, message);
}
OneError one_server_send_application_instance_get_status_request(OneServerPtr server,
                                                                 OneMessagePtr message) {
    return one::server_send_application_instance_get_status_request(server, message);
}

OneError one_server_send_application_instance_set_status_request(OneServerPtr server,
                                                                 OneMessagePtr message) {
    return one::server_send_application_instance_set_status_request(server, message);
}

OneError one_server_set_soft_stop_callback(OneServerPtr server,
                                           void (*callback)(void *data, int timeout),
                                           void *data) {
    return one::server_set_soft_stop_callback(server, callback, data);
}

OneError one_server_set_allocated_callback(OneServerPtr server,
                                           void (*callback)(void *data, void *array),
                                           void *data) {
    return one::server_set_allocated_callback(server, callback, data);
}

OneError one_server_set_meta_data_callback(OneServerPtr server,
                                           void (*callback)(void *data, void *array),
                                           void *data) {
    return one::server_set_meta_data_callback(server, callback, data);
}

OneError one_server_set_host_information_response_callback(
    OneServerPtr server, void (*callback)(void *, void *), void *data) {
    return one::server_set_host_information_response_callback(server, callback, data);
}

OneError one_server_set_application_instance_information_response_callback(
    OneServerPtr server, void (*callback)(void *, void *), void *data) {
    return one::server_set_application_instance_information_response_callback(
        server, callback, data);
}

OneError one_server_set_application_instance_get_status_response_callback(
    OneServerPtr server, void (*callback)(void *, int), void *data) {
    return one::server_set_application_instance_get_status_response_callback(
        server, callback, data);
}

OneError one_server_set_application_instance_set_status_response_callback(
    OneServerPtr server, void (*callback)(void *, int), void *data) {
    return one::server_set_application_instance_set_status_response_callback(
        server, callback, data);
}

void one_allocator_set_alloc(void *(callback)(unsigned int size)) {
    one::allocator_set_alloc(callback);
}

void one_allocator_set_free(void(callback)(void *)) {
    one::allocator_set_free(callback);
}

};  // extern "C"
