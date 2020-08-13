#pragma once

#include <utility>

#include <one/arcus/internal/platform.h>
#include <one/arcus/internal/opcodes.h>

#include <functional>
#include <string>
#include <utility>

namespace one {

// An array value that can be contained in a message's data.
class Array final {};

// An object value that can be contained in a message's data.
class Object final {};

// Payload provides abstraction for JSON data.
class Payload final {
public:
    Payload() = default;
    Payload(const Payload &) = delete;
    Payload &operator=(const Payload &) = delete;
    ~Payload() = default;

    void from_json(std::pair<const char *, size_t> data);
    std::pair<const char *, size_t> to_json() const;

    bool is_empty() const;
    void clear();

    bool is_val_int(const char *key) const;
    bool is_val_string(const char *key) const;
    bool is_val_array(const char *key) const;
    bool is_val_object(const char *key) const;

    // Getters.
    int val_int(const char *key, int &val) const;
    int val_string(const char *key, char **val) const;
    int val_string(const char *key, std::string &val) const;
    int val_array(const char *key, Array &val) const;
    int val_object(const char *key, Object &val) const;

    // Setters.
    int set_val_int(const char *key, int val);
    int set_val_string(const char *key, const char *val);
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
    Message(const Message &) = delete;
    Message &operator=(const Message &) = delete;
    ~Message() = default;

    int init(int code, std::pair<const char *, size_t> data);
    int init(Opcodes code, std::pair<const char *, size_t> data);
    int init(Opcodes code, const Payload &payload);

    void reset();

    Opcodes code() const;
    Payload &payload();
    const Payload &payload() const;

private:
    Opcodes _code;
    Payload _payload;
};

namespace messages {
int prepare_error_response(Message &message);
int prepare_soft_stop_request(int timeout, Message &message);
int prepare_allocated_request(Array &array, Message &message);
int prepare_meta_data_request(Array &array, Message &message);
int prepare_live_state_request(Message &message);
int prepare_live_state_response(int player, int max_player, const char *name, const char *map,
                                const char *mode, const char *version, Message &message);
int prepare_host_information_request(Message &message);
}  // namespace messages

}  // namespace one
