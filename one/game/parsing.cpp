#include <one/game/parsing.h>

#include <one/game/log.h>

namespace game {

bool Parsing::extract_key_value_payload(
    const OneArrayPtr array, std::function<bool(const size_t total_number_of_keys,
                                                const std::string &, const std::string &)>
                                 callback) {
    if (callback == nullptr) {
        L_ERROR("callback is nullptr");
        return false;
    }

    if (array == nullptr) {
        L_ERROR("array is nullptr");
        return false;
    }

    bool empty = true;
    auto err = one_array_is_empty(array, &empty);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    if (empty) {
        L_ERROR("array is empty");
        return false;
    }

    size_t number_of_keys = 0;
    err = one_array_size(array, &number_of_keys);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    OneObjectPtr pair = nullptr;
    err = one_object_create(&pair);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    for (unsigned int pos = 0; pos < number_of_keys; ++pos) {
        err = one_array_val_object(array, pos, pair);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            return false;
        }

        if (!extract_key_value_pair(pair, _key, _value)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            return false;
        }

        if (!callback(number_of_keys, _key.data(), _value.data())) {
            L_ERROR("callback failed");
            one_object_destroy(&pair);
            return false;
        }
    }

    one_object_destroy(&pair);
    return true;
}

bool Parsing::extract_key_value_pair(
    const OneObjectPtr pair, std::array<char, codec::key_max_size_null_terminated()> &key,
    std::array<char, codec::value_max_size_null_terminated()> &value) {
    if (pair == nullptr) {
        L_ERROR("pair is nullptr");
        return false;
    }

    size_t key_size = 0;
    auto err = one_object_val_string_size(pair, "key", &key_size);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    // Because buffer must add the '\0' explicitly.
    if (codec::key_max_size() < key_size + 1) {
        L_ERROR("key size is bigger than max size(" +
                std::to_string(codec::key_max_size()) + ")");
        return false;
    }

    err = one_object_val_string(pair, "key", key.data(), codec::key_max_size());
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    // Ensure that the string is `\0` terminated.
    _key[key_size] = '\0';

    size_t value_size = 0;
    err = one_object_val_string_size(pair, "value", &value_size);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    // Because buffer must add the '\0' explicitly.
    if (codec::value_max_size() < value_size + 1) {
        L_ERROR("value size is bigger than max size(" +
                std::to_string(codec::value_max_size()) + ")");
        return false;
    }

    err = one_object_val_string(pair, "value", value.data(), codec::value_max_size());
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    // Ensure that the string is `\0` terminated.
    _value[value_size] = '\0';
    return true;
}

bool Parsing::extract_string(const OneObjectPtr object, const char *key,
                             std::function<bool(const std::string &)> callback) {
    if (object == nullptr) {
        L_ERROR("object is nullptr");
        return false;
    }

    if (key == nullptr) {
        L_ERROR("key is nullptr");
        return false;
    }

    size_t size = 0;
    auto err = one_object_val_string_size(object, "serverName", &size);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    // Because buffer must add the '\0' explicitly.
    if (_string_buffer.size() < size + 1) {
        L_ERROR("string buffer size too small.");
        return false;
    }

    err = one_object_val_string(object, "serverName", _string_buffer.data(),
                                _string_buffer.size());
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    // Ensure that the string is `\0` terminated.
    _string_buffer[size] = '\0';

    if (!callback(_string_buffer.data())) {
        L_ERROR("callback failed");
        return false;
    }

    return true;
}

std::array<char, codec::key_max_size_null_terminated()> Parsing::_key = {};
std::array<char, codec::value_max_size_null_terminated()> Parsing::_value = {};
std::array<char, codec::string_buffer_max_size()> Parsing::_string_buffer = {};

}  // namespace game
