#pragma once

#include <one/arcus/c_api.h>
#include <one/arcus/c_error.h>
#include <one/game/log.h>

#include <array>
#include <functional>
#include <string>

// Forward declarations for One SDK object types.
struct OneArray;
typedef OneArray *OneArrayPtr;

struct OneObject;
typedef OneObject *OneObjectPtr;

namespace game {

namespace codec {

// For more details, go the to tab schema & expand all:
// https://www.i3d.net/docs/api/v3/game-publisher#/ApplicationInstance/updateApplicationInstanceGameEmptyAllocate
constexpr size_t key_max_size() {
    return 50;
}

constexpr size_t key_max_size_null_terminated() {
    return key_max_size() + 1;
}

// For more details, go the to tab schema & expand all:
// https://www.i3d.net/docs/api/v3/game-publisher#/ApplicationInstance/updateApplicationInstanceGameEmptyAllocate
constexpr size_t value_max_size() {
    return 2048;
}

// For more details, go the to tab schema & expand all:
// https://www.i3d.net/docs/api/v3/game-publisher#/ApplicationInstance/updateApplicationInstanceGameEmptyAllocate
constexpr size_t value_max_size_null_terminated() {
    return value_max_size() + 1;
}

constexpr size_t string_buffer_max_size() {
    return 2048;
}

constexpr size_t string_buffer_null_terminated() {
    return string_buffer_max_size() + 1;
}

}  // namespace codec

class Parsing final {
public:
    static bool extract_key_value_payload(
        const OneArrayPtr array,
        std::function<bool(const size_t total_number_of_keys, const std::string &key,
                           const std::string &value)>
            callback);

    static bool extract_key_value_pair(
        const OneObjectPtr pair,
        std::array<char, codec::key_max_size_null_terminated()> &key,
        std::array<char, codec::value_max_size_null_terminated()> &value);

    static bool extract_object_payload(const OneObjectPtr object,
                                       std::function<bool(void)> callback);

    static bool extract_string(const OneObjectPtr object, const char *key,
                               std::function<bool(const std::string &value)> callback);

private:
    static std::array<char, codec::key_max_size_null_terminated()> _key;
    static std::array<char, codec::value_max_size_null_terminated()> _value;
    static std::array<char, codec::string_buffer_max_size()> _string_buffer;

    Parsing() = delete;
    ~Parsing() = delete;
};

}  // namespace game
