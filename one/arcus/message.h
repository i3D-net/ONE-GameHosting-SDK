#include <utility>

#include <one/arcus/internal/platform.h>
#include <one/arcus/internal/opcodes.h>

#include <functional>
#include <string>

namespace one {

// An array value that can be contained in a message's data.
class Array final {};

// An object value that can be contained in a message's data.
class Object final {};

// Payload provides abstraction for JSON data.
class Payload final {
public:
    Payload() = default;
    ~Payload() = default;

    void from_json(const char *, size_t);
    std::pair<const char *, size_t> to_json() const;

    bool is_empty() const;
    void clear();

    bool is_val_int(const char *key) const;
    bool is_val_string(const char *key) const;
    bool is_val_array(const char *key) const;
    bool is_val_object(const char *key) const;

    // Getters.
    int val_int(const char *key, int &val) const;
    int val_string(const char *key, std::string &val) const;
    int val_array(const char *key, Array &val) const;
    int val_object(const char *key, Object &val) const;

    // Setters.
    int set_val_int(const char *key, int val);
    int set_val_string(const char *key, const std::string &val);
    int set_val_array(const char *key, const Array &val);
    int set_val_object(const char *key, const Object &val);

private:
    // TBD
    // RapidJson::Document
};

// Todo: would a simple struct be better for now? Consider memory pooling both
// for arcus message and for rapidjson internal data.
class Message final {
public:
    Message();
    ~Message() = default;

    int init(int code, const char *data, size_t size);
    int init(Opcodes code, const char *data, size_t size);

    void reset();

    Opcodes code() const;
    const Payload &payload() const;

private:
    Opcodes _code;
    Payload _payload;
};

}  // namespace one
