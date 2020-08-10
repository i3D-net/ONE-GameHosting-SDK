#include <one/arcus/message.h>

#include <one/arcus/internal/opcodes.h>

namespace one {

void Payload::from_json(std::pair<const char *, size_t> data) {}

std::pair<const char *, size_t> Payload::to_json() const {
    return std::pair<const char *, size_t>();
}

bool Payload::is_empty() const {
    return true;
}

void Payload::clear() {}

bool Payload::is_val_int(const char *key) const {
    return 0;
}

bool Payload::is_val_string(const char *key) const {
    return 0;
}

bool Payload::is_val_array(const char *key) const {
    return 0;
}

bool Payload::is_val_object(const char *key) const {
    return 0;
}

int Payload::val_int(const char *key, int &val) const {
    return 0;
}

int Payload::val_string(const char *key, char **val) const {
    return 0;
}

int Payload::val_string(const char *key, std::string &val) const {
    return 0;
}

int Payload::val_array(const char *key, Array &val) const {
    return 0;
}

int Payload::val_object(const char *key, Object &val) const {
    return 0;
}

int Payload::set_val_int(const char *key, int val) {
    return 0;
}

int Payload::set_val_string(const char *key, const char *val) {
    return 0;
}

int Payload::set_val_string(const char *key, const std::string &val) {
    return 0;
}

int Payload::set_val_array(const char *key, const Array &val) {
    return 0;
}

int Payload::set_val_object(const char *key, const Object &val) {
    return 0;
}

Message::Message() : _code(Opcodes::invalid) {}

int Message::init(int code, std::pair<const char *, size_t> data) {
    return init(static_cast<Opcodes>(code), data);
}

int Message::init(Opcodes code, std::pair<const char *, size_t> data) {
    _code = code;
    _payload.from_json(data);
    return 0;
}

int Message::init(Opcodes code, const Payload &payload) {
    _code = code;
    _payload = payload;
    return 0;
}

void Message::reset() {
    _code = Opcodes::invalid;
    _payload.clear();
}

Opcodes Message::code() const {
    return _code;
}

Payload &Message::payload() {
    return _payload;
}

const Payload &Message::payload() const {
    return _payload;
}

namespace messages {

int prepare_live_state(int player, int max_player, const char *name, const char *map,
                       const char *mode, const char *version, Message &message) {
    Payload payload;
    int error = payload.set_val_int("player", player);
    if (error != 0) {
        return -1;
    }
    error = payload.set_val_int("max_player", max_player);
    if (error != 0) {
        return -1;
    }
    error = payload.set_val_string("name", name);
    if (error != 0) {
        return -1;
    }
    error = payload.set_val_string("map", map);
    if (error != 0) {
        return -1;
    }
    error = payload.set_val_string("mode", mode);
    if (error != 0) {
        return -1;
    }
    error = payload.set_val_string("version", version);
    if (error != 0) {
        return -1;
    }

    message.reset();
    message.init(Opcodes::live_state, payload);

    return 0;
}

}  // namespace messages

}  // namespace one
