#include <utility>

#include <one/arcus/internal/platform.h>

namespace one {

// An array value that can be contained in a message's data.
class Array final {};

// An object value that can be contained in a message's data.
class Object final {};

// Payload provides abstraction for JSON data.
class Payload final {
public:
    Payload();

    void from_json(const char*, size_t);
    std::pair<const char*, size_t> to_json() const;

    // Getters.
    int val_int(const char* key, int* error);
    const char* val_string(const char* key, int* error);
    Array& val_array(const char* key, int* error);
    Object& val_object(const char* key, int* error);

    // Setters.
    void set_val_int(const char* key, int val, int* error);
    void set_val_string(const char* key, const char* val, int* error);
    void set_val_array(const char* key, const Array& val, int* error);
    void set_val_object(const char* key, const Object& val, int* error);

private:
    // TBD
    // RapidJson::Document
};

// Todo: would a simple struct be better for now? Consider memory pooling both
// for arcus message and for rapidjson internal data.
class Message final {
public:
    Message();
    Message(int code);

    int set_code(int code);
    int code();

    Payload& payload();

private:
    Payload _payload;
};

}  // namespace one