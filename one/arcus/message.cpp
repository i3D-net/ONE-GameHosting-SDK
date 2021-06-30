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

OneError Payload::from_json(std::pair<const char *, size_t> data) {
    rapidjson::ParseResult ok = _doc.Parse(data.first, data.second);
    if (!ok) {
        return ONE_ERROR_PAYLOAD_PARSE_FAILED;
    }

    return ONE_ERROR_NONE;
}

String Payload::to_json() const {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    _doc.Accept(writer);
    return String(buffer.GetString(), buffer.GetSize());
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

OneError Payload::val_bool(const char *key, bool &val) const {
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

OneError Payload::val_int(const char *key, int &val) const {
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

OneError Payload::val_string(const char *key, String &val) const {
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

OneError Payload::val_array(const char *key, Array &val) const {
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

OneError Payload::val_object(const char *key, Object &val) const {
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

OneError Payload::val_root_object(Object &val) const {
    if (!_doc.IsObject()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_OBJECT;
    }

    val.set(_doc);
    return ONE_ERROR_NONE;
}

OneError Payload::set_val_bool(const char *key, bool val) {
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

OneError Payload::set_val_int(const char *key, int val) {
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

OneError Payload::set_val_string(const char *key, const String &val) {
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

OneError Payload::set_val_array(const char *key, const Array &val) {
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

OneError Payload::set_val_object(const char *key, const Object &val) {
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

OneError Payload::set_val_root_object(const Object &val) {
    if (!_doc.IsObject()) {
        return ONE_ERROR_PAYLOAD_WRONG_TYPE_IS_EXPECTING_OBJECT;
    }

    _doc.CopyFrom(val.get(), _doc.GetAllocator());
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

OneError Message::init(Opcode code, std::pair<const char *, size_t> data) {
    _code = code;
    auto err = _payload.from_json(data);
    if (is_error(err)) {
        _code = Opcode::invalid;
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError Message::init(Opcode code, const Payload &payload) {
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

OneError prepare_soft_stop(int timeout, Message &message) {
    Payload payload;
    auto err = payload.set_val_int("timeout", timeout);
    if (is_error(err)) {
        return err;
    }

    message.reset();
    err = message.init(Opcode::soft_stop, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError prepare_allocated(const Array &array, Message &message) {
    Payload payload;
    auto err = payload.set_val_array("data", array);
    if (is_error(err)) {
        return err;
    }

    message.reset();
    err = message.init(Opcode::allocated, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError prepare_metadata(const Array &array, Message &message) {
    Payload payload;
    auto err = payload.set_val_array("data", array);
    if (is_error(err)) {
        return err;
    }

    message.reset();
    err = message.init(Opcode::metadata, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError prepare_reverse_metadata(const Array &array, Message &message) {
    Payload payload;
    auto err = payload.set_val_array("data", array);
    if (is_error(err)) {
        return err;
    }

    message.reset();
    err = message.init(Opcode::reverse_metadata, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError

prepare_live_state(int players, int max_players, const char *name, const char *map,
                   const char *mode, const char *version, Object *additional_data,
                   Message &message) {
    Payload payload;

    auto err = ONE_ERROR_NONE;

    if (additional_data != nullptr) {
        err = payload.set_val_root_object(*additional_data);
        if (is_error(err)) {
            return err;
        }
    }

    err = payload.set_val_int("players", players);
    if (is_error(err)) {
        return err;
    }

    err = payload.set_val_int("maxPlayers", max_players);
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
    err = message.init(Opcode::live_state, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError prepare_host_information(const Object &information, Message &message) {
    Payload payload;
    auto err = payload.set_val_root_object(information);
    if (is_error(err)) {
        return err;
    }

    err = message.init(Opcode::host_information, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError prepare_application_instance_information(const Object &information,
                                                  Message &message) {
    Payload payload;
    auto err = payload.set_val_root_object(information);
    if (is_error(err)) {
        return err;
    }

    err = message.init(Opcode::application_instance_information, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError prepare_application_instance_status(int status, Message &message) {
    Payload payload;
    auto err = payload.set_val_int("status", status);
    if (is_error(err)) {
        return err;
    }

    err = message.init(Opcode::application_instance_status, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError prepare_custom_command(const Array &array, Message &message) {
    Payload payload;
    auto err = payload.set_val_array("data", array);
    if (is_error(err)) {
        return err;
    }

    message.reset();
    err = message.init(Opcode::custom_command, payload);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

}  // namespace messages

// namespace messages

}  // namespace one
}  // namespace i3d
