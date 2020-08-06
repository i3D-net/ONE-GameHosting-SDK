#include <one/arcus/message.h>

#include <one/arcus/internal/opcodes.h>

namespace one {

void Payload::from_json(const char*, size_t) {}

std::pair<const char*, size_t> Payload::to_json() const {
    return std::pair<const char*, size_t>();
}

bool Payload::is_empty() const {
    return true;
}

void Payload::clear() {}

bool Payload::is_val_int(const char* key) const {
    return 0;
}

bool Payload::is_val_string(const char* key) const {
    return 0;
}

bool Payload::is_val_array(const char* key) const {
    return 0;
}

bool Payload::is_val_object(const char* key) const {
    return 0;
}

int Payload::val_int(const char* key, int& val) const {
    return 0;
}

int Payload::val_string(const char* key, std::string& val) const {
    return 0;
}

int Payload::val_array(const char* key, Array& val) const {
    return 0;
}

int Payload::val_object(const char* key, Object& val) const {
    return 0;
}

int Payload::set_val_int(const char* key, int val) {
    return 0;
}

int Payload::set_val_string(const char* key, const std::string& val) {
    return 0;
}

int Payload::set_val_array(const char* key, const Array& val) {
    return 0;
}

int Payload::set_val_object(const char* key, const Object& val) {
    return 0;
}

Message::Message() : _code(Opcodes::invalid) {}

int Message::init(int code, const char* data, size_t size) {
    return init(static_cast<Opcodes>(code), data, size);
}

int Message::init(Opcodes code, const char* data, size_t size) {
    _code = code;
    _payload.from_json(data, size);
    return 0;
}

void Message::reset() {
    _code = Opcodes::invalid;
    _payload.clear();
}

Opcodes Message::code() const {
    return _code;
}

const Payload& Message::payload() const {
    return _payload;
}

}  // namespace one
