#include <one/arcus/message.h>

#include <one/arcus/array.h>
#include <one/arcus/internal/rapidjson/stringbuffer.h>
#include <one/arcus/internal/rapidjson/writer.h>
#include <one/arcus/opcode.h>
#include <one/arcus/object.h>

namespace i3d {
namespace one {

Payload::Payload() : _doc(rapidjson::kObjectType) {}

Payload::Payload(const Payload &other) {
    _doc.CopyFrom(other._doc, _doc.GetAllocator());
}

Payload &Payload::operator=(const Payload &other) {
    _doc.CopyFrom(other._doc, _doc.GetAllocator());
    return *this;
}

Error Payload::from_json(std::pair<const char *, size_t> data) {
    rapidjson::ParseResult ok = _doc.Parse(data.first, data.second);
    if (!ok) {
        return ONE_ERROR_PAYLOAD_PARSE_FAILED;
    }

    return ONE_ERROR_NONE;
}

std::string Payload::to_json() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    _doc.Accept(writer);
    return std::string(buffer.GetString(), buffer.GetSize());
}

bool Payload::is_empty() const {
    return _doc.ObjectEmpty();
}

void Payload::clear() {
    _doc.SetObject();
}

bool Payload::is_val_bool(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return false;
    }

    return value->value.IsBool();
}

bool Payload::is_val_int(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return false;
    }

    return value->value.IsInt();
}

bool Payload::is_val_string(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return false;
    }

    return value->value.IsString();
}

bool Payload::is_val_array(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return false;
    }

    return value->value.IsArray();
}

bool Payload::is_val_object(const char *key) const {
    if (key == nullptr) {
        return false;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return false;
    }

    return value->value.IsObject();
}

Error Payload::val_bool(const char *key, bool &val) const {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return ONE_ERROR_PAYLOAD_KEY_NOT_FOUND;
    }

    if (!value->value.IsBool()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_BOOL;
    }

    val = value->value.GetBool();
    return ONE_ERROR_NONE;
}

Error Payload::val_int(const char *key, int &val) const {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return ONE_ERROR_PAYLOAD_KEY_NOT_FOUND;
    }

    if (!value->value.IsInt()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_INT;
    }
    val = value->value.GetInt();
    return ONE_ERROR_NONE;
}

Error Payload::val_string(const char *key, std::string &val) const {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return ONE_ERROR_PAYLOAD_KEY_NOT_FOUND;
    }

    if (!value->value.IsString()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_STRING;
    }

    val = value->value.GetString();
    return ONE_ERROR_NONE;
}

Error Payload::val_array(const char *key, Array &val) const {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return ONE_ERROR_PAYLOAD_KEY_NOT_FOUND;
    }

    if (!value->value.IsArray()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_ARRAY;
    }

    val.set(value->value);
    return ONE_ERROR_NONE;
}

Error Payload::val_object(const char *key, Object &val) const {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);
    if (value == _doc.MemberEnd()) {
        return ONE_ERROR_PAYLOAD_KEY_NOT_FOUND;
    }

    if (!value->value.IsObject()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_OBJECT;
    }

    val.set(value->value);
    return ONE_ERROR_NONE;
}

Error Payload::set_val_bool(const char *key, bool val) {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);

    // Adding key if it does not exists already.
    if (value == _doc.MemberEnd()) {
        _doc.AddMember(rapidjson::Value(key, _doc.GetAllocator()).Move(), val,
                       _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoiding changing an existing key type.
    if (!value->value.IsBool()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_BOOL;
    }

    value->value.SetBool(val);
    return ONE_ERROR_NONE;
}

Error Payload::set_val_int(const char *key, int val) {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);

    // Adding key if it does not exists already.
    if (value == _doc.MemberEnd()) {
        _doc.AddMember(rapidjson::Value(key, _doc.GetAllocator()).Move(), val,
                       _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoiding changing an existing key type.
    if (!value->value.IsInt()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_INT;
    }

    value->value.SetInt(val);
    return ONE_ERROR_NONE;
}

Error Payload::set_val_string(const char *key, const std::string &val) {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);

    // Adding key if it does not exists already.
    if (value == _doc.MemberEnd()) {
        _doc.AddMember(rapidjson::Value(key, _doc.GetAllocator()).Move(),
                       rapidjson::Value(val.c_str(), _doc.GetAllocator()).Move(),
                       _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoiding changing an existing key type.
    if (!value->value.IsString()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_STRING;
    }

    value->value.SetString(val.c_str(), _doc.GetAllocator());
    return ONE_ERROR_NONE;
}

Error Payload::set_val_array(const char *key, const Array &val) {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);

    // Adding key if it does not exists already.
    if (value == _doc.MemberEnd()) {
        _doc.AddMember(rapidjson::Value(key, _doc.GetAllocator()).Move(),
                       rapidjson::Value(val.get(), _doc.GetAllocator()).Move(),
                       _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoiding changing an existing key type.
    if (!value->value.IsArray()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_ARRAY;
    }

    value->value.CopyFrom(val.get(), _doc.GetAllocator());
    return ONE_ERROR_NONE;
}

Error Payload::set_val_object(const char *key, const Object &val) {
    if (key == nullptr) {
        return ONE_ERROR_PAYLOAD_KEY_IS_NULLPTR;
    }

    const auto &value = _doc.FindMember(key);

    // Adding key if it does not exists already.
    if (value == _doc.MemberEnd()) {
        _doc.AddMember(rapidjson::Value(key, _doc.GetAllocator()).Move(),
                       rapidjson::Value(val.get(), _doc.GetAllocator()).Move(),
                       _doc.GetAllocator());
        return ONE_ERROR_NONE;
    }

    // Avoiding changing an existing key type.
    if (!value->value.IsObject()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_OBJECT;
    }

    value->value.CopyFrom(val.get(), _doc.GetAllocator());
    return ONE_ERROR_NONE;
}

Message::Message() : _code(Opcode::invalid) {}

Message::Message(const Message &other) {
    _code = other.code();
    _payload = other.payload();
}

Message &Message::operator=(const Message &other) {
    _code = other.code();
    _payload = other.payload();
    return *this;
}

Error Message::init(Opcode code, std::pair<const char *, size_t> data) {
    _code = code;
    auto err = _payload.from_json(data);
    if (is_error(err)) {
        _code = Opcode::invalid;
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Message::init(Opcode code, const Payload &payload) {
    _code = code;
    _payload = payload;
    return ONE_ERROR_NONE;
}

void Message::reset() {
    _code = Opcode::invalid;
    _payload.clear();
}

Opcode Message::code() const {
    return _code;
}

Payload &Message::payload() {
    return _payload;
}

const Payload &Message::payload() const {
    return _payload;
}

namespace messages {

Error prepare_error_response(Message &message) {
    message.reset();
    auto err = message.init(Opcode::error_response, Payload());
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error prepare_soft_stop_request(int timeout, Message &message) {
    Payload payload;
    auto err = payload.set_val_int("timeout", timeout);
    if (is_error(err)) {
        return err;
    }

    message.reset();
    err = message.init(Opcode::soft_stop_request, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error prepare_allocated_request(Array &array, Message &message) {
    Payload payload;
    auto err = payload.set_val_array("data", array);
    if (is_error(err)) {
        return err;
    }

    message.reset();
    err = message.init(Opcode::allocated_request, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error prepare_meta_data_request(Array &array, Message &message) {
    Payload payload;
    auto err = payload.set_val_array("data", array);
    if (is_error(err)) {
        return err;
    }

    message.reset();
    err = message.init(Opcode::meta_data_request, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error prepare_live_state_request(Message &message) {
    auto err = message.init(Opcode::live_state_request, Payload());
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error prepare_live_state_response(int player, int max_player, const char *name,
                                  const char *map, const char *mode, const char *version,
                                  Message &message) {
    Payload payload;
    auto err = payload.set_val_int("player", player);
    if (is_error(err)) {
        return err;
    }

    err = payload.set_val_int("max_player", max_player);
    if (is_error(err)) {
        return err;
    }

    err = payload.set_val_string("name", name);
    if (is_error(err)) {
        return err;
    }

    err = payload.set_val_string("map", map);
    if (is_error(err)) {
        return err;
    }

    err = payload.set_val_string("mode", mode);
    if (is_error(err)) {
        return err;
    }

    err = payload.set_val_string("version", version);
    if (is_error(err)) {
        return err;
    }

    message.reset();
    err = message.init(Opcode::live_state_response, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error prepare_host_information_request(Message &message) {
    auto err = message.init(Opcode::host_information_request, Payload());
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

}  // namespace messages

// namespace messages

}  // namespace one
}  // namespace i3d