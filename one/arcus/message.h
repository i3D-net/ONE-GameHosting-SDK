#pragma once

#include <functional>
#include <utility>

#include <one/arcus/error.h>
#include <one/arcus/c_platform.h>
#include <one/arcus/internal/rapidjson/document.h>
#include <one/arcus/opcode.h>
#include <one/arcus/types.h>

namespace rapidjson = RAPIDJSON_NAMESPACE;

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

    OneError from_json(std::pair<const char *, size_t> data);
    String to_json() const;

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
    OneError val_bool(const char *key, bool &val) const;
    OneError val_int(const char *key, int &val) const;
    OneError val_string(const char *key, String &val) const;
    OneError val_array(const char *key, Array &val) const;
    OneError val_object(const char *key, Object &val) const;
    OneError val_root_object(Object &val) const;

    // Setters.
    OneError set_val_bool(const char *key, bool val);
    OneError set_val_int(const char *key, int val);
    OneError set_val_string(const char *key, const String &val);
    OneError set_val_array(const char *key, const Array &val);
    OneError set_val_object(const char *key, const Object &val);
    OneError set_val_root_object(const Object &val);

private:
    rapidjson::Document _doc;
};

class Message final {
public:
    Message();
    Message(const Message &other);
    Message &operator=(const Message &other);
    ~Message() = default;

    OneError init(Opcode code, std::pair<const char *, size_t> data);
    OneError init(Opcode code, const Payload &payload);

    void reset();

    Opcode code() const;
    Payload &payload();
    const Payload &payload() const;

private:
    Opcode _code;
    Payload _payload;
};

namespace messages {
OneError prepare_soft_stop(int timeout, Message &message);
OneError prepare_allocated(const Array &array, Message &message);
OneError prepare_metadata(const Array &array, Message &message);
OneError prepare_reverse_metadata(const Array &array, Message &message);
OneError prepare_live_state(int players, int max_players, const char *name,
                            const char *map, const char *mode, const char *version,
                            Object *additional_data, Message &message);
OneError prepare_host_information(const Object &information, Message &message);
OneError prepare_application_instance_information(const Object &information,
                                                  Message &message);
OneError prepare_application_instance_status(int status, Message &message);
OneError prepare_custom_command(const Array &array, Message &message);

}  // namespace messages

}  // namespace one
}  // namespace i3d
