#include <one/arcus/object.h>

#include <one/arcus/array.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/internal/rapidjson/stringbuffer.h>
#include <one/arcus/internal/rapidjson/writer.h>
#include <one/arcus/opcode.h>

#include <cstring>

namespace one {

Object::Object() : _doc(rapidjson::kObjectType) {}

Object::Object(const Object &other) {
    _doc.CopyFrom(other.get(), _doc.GetAllocator());
}

Object &Object::operator=(const Object &other) {
    _doc.CopyFrom(other.get(), _doc.GetAllocator());
    return *this;
}

Error Object::set(const rapidjson::Value &object) {
    if (!object.IsObject()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_OBJECT;
    }

    _doc.CopyFrom(object, _doc.GetAllocator());
    return ONE_ERROR_NONE;
}

void Object::clear() {
    _doc.SetObject();
}

bool Object::is_empty() const {
    return _doc.ObjectEmpty();
}

Error Object::remove_key(const char *key) {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return ONE_ERROR_OBJECT_KEY_NOT_FOUND;
    }

    _doc.RemoveMember(member);
    return ONE_ERROR_NONE;
}

bool Object::is_val_bool(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return false;
    }

    return member->value.IsBool();
}

bool Object::is_val_int(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return false;
    }

    return member->value.IsInt();
}

bool Object::is_val_string(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return false;
    }

    return member->value.IsString();
}

bool Object::is_val_array(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return false;
    }

    return member->value.IsArray();
}

bool Object::is_val_object(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return false;
    }

    return member->value.IsObject();
}

Error Object::val_bool(const char *key, bool &val) const {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return ONE_ERROR_OBJECT_KEY_NOT_FOUND;
    }

    if (!member->value.IsBool()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_BOOL;
    }

    val = member->value.GetBool();
    return ONE_ERROR_NONE;
}

Error Object::val_int(const char *key, int &val) const {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return ONE_ERROR_OBJECT_KEY_NOT_FOUND;
    }

    if (!member->value.IsInt()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_INT;
    }

    val = member->value.GetInt();
    return ONE_ERROR_NONE;
}

Error Object::val_string(const char *key, std::string &val) const {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return ONE_ERROR_OBJECT_KEY_NOT_FOUND;
    }

    if (!member->value.IsString()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_STRING;
    }

    val = member->value.GetString();
    return ONE_ERROR_NONE;
}

Error Object::val_array(const char *key, Array &val) const {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return ONE_ERROR_OBJECT_KEY_NOT_FOUND;
    }

    if (!member->value.IsArray()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_ARRAY;
    }

    auto err = val.set(member->value);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Object::val_object(const char *key, Object &val) const {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        return ONE_ERROR_OBJECT_KEY_NOT_FOUND;
    }

    if (!member->value.IsObject()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_OBJECT;
    }

    auto err = val.set(member->value);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Object::set_val_bool(const char *key, bool val) {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        _doc.AddMember(rapidjson::StringRef(key), val, _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoid changing the current element type.
    if (!member->value.IsBool()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_BOOL;
    }

    member->value.SetBool(val);
    return ONE_ERROR_NONE;
}

Error Object::set_val_int(const char *key, int val) {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        _doc.AddMember(rapidjson::StringRef(key), val, _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoid changing the current element type.
    if (!member->value.IsInt()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_INT;
    }

    member->value.SetInt(val);
    return ONE_ERROR_NONE;
}

Error Object::set_val_string(const char *key, const std::string &val) {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        _doc.AddMember(rapidjson::StringRef(key), rapidjson::StringRef(val.c_str()),
                       _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoid changing the current element type.
    if (!member->value.IsString()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_STRING;
    }

    member->value.Set(val.c_str());
    return ONE_ERROR_NONE;
}

Error Object::set_val_array(const char *key, const Array &val) {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        rapidjson::Value value;
        value.CopyFrom(val.get(), _doc.GetAllocator());
        _doc.AddMember(rapidjson::StringRef(key), value, _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoid changing the current element type.
    if (!member->value.IsArray()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_ARRAY;
    }

    member->value.CopyFrom(val.get(), _doc.GetAllocator());
    return ONE_ERROR_NONE;
}

Error Object::set_val_object(const char *key, const Object &val) {
    if (key == nullptr) {
        return ONE_ERROR_OBJECT_KEY_IS_NULLPTR;
    }

    const auto &member = _doc.FindMember(key);
    if (member == _doc.MemberEnd()) {
        rapidjson::Value value;
        value.CopyFrom(val.get(), _doc.GetAllocator());
        _doc.AddMember(rapidjson::StringRef(key), value, _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoid changing the current element type.
    if (!member->value.IsObject()) {
        return ONE_ERROR_OBJECT_WRONG_TYPE_IS_EXPECTING_OBJECT;
    }

    member->value.CopyFrom(val.get(), _doc.GetAllocator());
    return ONE_ERROR_NONE;
}

}  // namespace one
