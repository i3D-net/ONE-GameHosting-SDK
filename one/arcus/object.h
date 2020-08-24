#pragma once

#include <one/arcus/error.h>
#include <one/arcus/internal/rapidjson/document.h>

#include <functional>
#include <string>
#include <utility>

namespace one {

class Array;

// An object value that can be contained in a message's data.
class Object final {
public:
    Object();
    Object(const Object &other);
    Object &operator=(const Object &other);
    ~Object() = default;

    Error set(const rapidjson::Value &object);
    const rapidjson::Value &get() const {
        return _doc;
    }

    // Object management.
    void clear();
    bool is_empty() const;
    Error remove_key(const char *key);

    // Type checks.
    bool is_val_bool(const char *key) const;
    bool is_val_int(const char *key) const;
    bool is_val_string(const char *key) const;
    bool is_val_array(const char *key) const;
    bool is_val_object(const char *key) const;

    // Getters.
    Error val_bool(const char *key, bool &val) const;
    Error val_int(const char *key, int &val) const;
    Error val_string_size(const char *key, size_t &size) const;
    Error val_string(const char *key, std::string &val) const;
    Error val_array(const char *key, Array &val) const;
    Error val_object(const char *key, Object &val) const;

    // Setters.
    Error set_val_bool(const char *key, bool val);
    Error set_val_int(const char *key, int val);
    Error set_val_string(const char *key, const std::string &val);
    Error set_val_array(const char *key, const Array &val);
    Error set_val_object(const char *key, const Object &val);

private:
    rapidjson::Document _doc;
};

}  // namespace one
