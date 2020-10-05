#pragma once

#include <one/arcus/error.h>
#include <one/arcus/internal/platform.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/opcode.h>

#include <functional>
#include <string>
#include <utility>

namespace i3d {
namespace one {

class Array;
class Object;

// Payload provides abstraction for JSON data.
class Payload final {
public:
    Payload();
    Payload(const Payload &other);
    Payload &operator=(const Payload &other);
    ~Payload() = default;

    Error from_json(std::pair<const char *, size_t> data);
    std::string to_json() const;

    const rapidjson::Value &get() const {
        return _doc;
    }

    bool is_empty() const;
    void clear();

    bool is_val_bool(const char *key) const;
    bool is_val_int(const char *key) const;
    bool is_val_string(const char *key) const;
    bool is_val_array(const char *key) const;
    bool is_val_object(const char *key) const;

    // Getters.
    Error val_bool(const char *key, bool &val) const;
    Error val_int(const char *key, int &val) const;
    Error val_string(const char *key, std::string &val) const;
    Error val_array(const char *key, Array &val) const;
    Error val_object(const char *key, Object &val) const;
    Error val_root_object(Object &val) const;

    // Setters.
    Error set_val_bool(const char *key, bool val);
    Error set_val_int(const char *key, int val);
    Error set_val_string(const char *key, const std::string &val);
    Error set_val_array(const char *key, const Array &val);
    Error set_val_object(const char *key, const Object &val);
    Error set_val_root_object(const Object &val);

private:
    rapidjson::Document _doc;
};

// Todo: would a simple struct be better for now? Consider memory pooling both
// for arcus message and for rapidjson internal data.
class Message final {
public:
    Message();
    Message(const Message &other);
    Message &operator=(const Message &other);
    ~Message() = default;

    Error init(Opcode code, std::pair<const char *, size_t> data);
    Error init(Opcode code, const Payload &payload);

    void reset();

    Opcode code() const;
    Payload &payload();
    const Payload &payload() const;

private:
    Opcode _code;
    Payload _payload;
};

namespace messages {
Error prepare_soft_stop(int timeout, Message &message);
Error prepare_allocated(const Array &array, Message &message);
Error prepare_metadata(const Array &array, Message &message);
Error prepare_live_state(int players, int max_players, const char *name,
                                  const char *map, const char *mode, const char *version,
                                  Message &message);
Error prepare_host_information_response(const Object &information, Message &message);
Error prepare_application_instance_information_response(const Object &information,
                                                        Message &message);
Error prepare_application_instance_set_status_request(int status, Message &message);

}  // namespace messages

}  // namespace one
}  // namespace i3d
