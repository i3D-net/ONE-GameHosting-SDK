#pragma once

#include <functional>
#include <utility>

#include <one/arcus/error.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/types.h>

namespace rapidjson = RAPIDJSON_NAMESPACE;

namespace i3d {
namespace one {

class Array;

// An object value that can be contained in a message's data.
class Object final {
public:
    Object();
    Object(const Object &other);
    Object &operator=(const Object &other);
    ~Object() = default;

    OneError set(const rapidjson::Value &object);
    const rapidjson::Value &get() const {
        return _doc;
    }

    // Object management.
    void clear();
    bool is_empty() const;
    OneError remove_key(const char *key);

    // Type checks.
    bool is_val_bool(const char *key) const;
    bool is_val_int(const char *key) const;
    bool is_val_string(const char *key) const;
    bool is_val_array(const char *key) const;
    bool is_val_object(const char *key) const;

    // Getters.
    OneError val_bool(const char *key, bool &val) const;
    OneError val_int(const char *key, int &val) const;
    OneError val_string_size(const char *key, size_t &size) const;
    OneError val_string(const char *key, String &val) const;
    OneError val_array(const char *key, Array &val) const;
    OneError val_object(const char *key, Object &val) const;

    // Setters.
    OneError set_val_bool(const char *key, bool val);
    OneError set_val_int(const char *key, int val);
    OneError set_val_string(const char *key, const String &val);
    OneError set_val_array(const char *key, const Array &val);
    OneError set_val_object(const char *key, const Object &val);

private:
    rapidjson::Document _doc;
};

}  // namespace one
}  // namespace i3d