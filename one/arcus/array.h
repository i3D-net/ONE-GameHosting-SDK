#pragma once

#include <one/arcus/error.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/types.h>

#include <functional>
#include <utility>

namespace i3d {
namespace one {

class Object;

// An array value that can be contained in a message's data.
class Array final {
public:
    Array();
    Array(const Array &other);
    Array &operator=(const Array &other);
    ~Array() = default;

    Error set(const rapidjson::Value &array);
    const rapidjson::Value &get() const {
        return _doc;
    }

    // Array management.
    void clear();
    void reserve(size_t size);
    bool is_empty() const;
    size_t size() const;
    size_t capacity() const;

    void push_back_bool(bool val);
    void push_back_int(int val);
    void push_back_string(const String &val);
    void push_back_array(const Array &val);
    void push_back_object(const Object &val);

    void pop_back();

    // Type checks.
    bool is_val_bool(unsigned int pos) const;
    bool is_val_int(unsigned int pos) const;
    bool is_val_string(unsigned int pos) const;
    bool is_val_array(unsigned int pos) const;
    bool is_val_object(unsigned int pos) const;

    // Getters.
    Error val_bool(unsigned int pos, bool &val) const;
    Error val_int(unsigned int pos, int &val) const;
    Error val_string_size(unsigned int pos, size_t &size) const;
    Error val_string(unsigned int pos, String &val) const;
    Error val_array(unsigned int pos, Array &val) const;
    Error val_object(unsigned int pos, Object &val) const;

    // Setters.
    Error set_val_bool(unsigned int pos, bool val);
    Error set_val_int(unsigned int pos, int val);
    Error set_val_string(unsigned int pos, const String &val);
    Error set_val_array(unsigned int pos, const Array &val);
    Error set_val_object(unsigned int pos, const Object &val);

private:
    rapidjson::Document _doc;
};

}  // namespace one
}  // namespace i3d