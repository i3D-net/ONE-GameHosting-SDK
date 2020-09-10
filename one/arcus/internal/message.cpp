#include <one/arcus/internal/message.h>

#include <one/arcus/opcode.h>
#include <one/arcus/message.h>

#include <functional>

namespace i3d {
namespace one {

namespace validation {

Error error_response(const Message &message, params::ErrorResponse &) {
    const auto code = message.code();

    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::error_response) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_ERROR_RESPONSE;
    }

    if (!message.payload().is_empty()) {
        return ONE_ERROR_MESSAGE_OPCODE_PAYLOAD_NOT_EMPTY;
    }

    return ONE_ERROR_NONE;
}

Error soft_stop_request(const Message &message, params::SoftStopRequest &params) {
    const auto code = message.code();

    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::soft_stop_request) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_SOFT_STOP_REQUEST;
    }

    const auto &payload = message.payload();

    const auto err = payload.val_int("timeout", params._timeout);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error allocated_request(const Message &message, params::AllocatedRequest &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::allocated_request) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_ALLOCATED_REQUEST;
    }

    const auto &payload = message.payload();

    auto err = payload.val_array("data", params._data);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error meta_data_request(const Message &message, params::MetaDataRequest &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::meta_data_request) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_META_DATA_REQUEST;
    }

    const auto &payload = message.payload();

    auto err = payload.val_array("data", params._data);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error live_state_request(const Message &message, params::LiveStateRequest &) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::live_state_request) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_LIVE_STATE_REQUEST;
    }

    if (!message.payload().is_empty()) {
        return ONE_ERROR_MESSAGE_OPCODE_PAYLOAD_NOT_EMPTY;
    }

    return ONE_ERROR_NONE;
}

Error live_state_response(const Message &message, params::LiveStateResponse &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::live_state_response) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_LIVE_STATE_RESPONSE;
    }

    const auto &payload = message.payload();

    auto err = payload.val_int("players", params._players);
    if (is_error(err)) {
        return err;
    }

    err = payload.val_int("maxPlayers", params._max_players);
    if (is_error(err)) {
        return err;
    }

    err = payload.val_string("name", params._name);
    if (is_error(err)) {
        return err;
    }

    err = payload.val_string("map", params._map);
    if (is_error(err)) {
        return err;
    }

    err = payload.val_string("mode", params._mode);
    if (is_error(err)) {
        return err;
    }

    err = payload.val_string("version", params._version);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error host_information_request(const Message &message, params::HostInformationRequest &) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::host_information_request) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_HOST_INFORMATION_REQUEST;
    }

    if (!message.payload().is_empty()) {
        return ONE_ERROR_MESSAGE_OPCODE_PAYLOAD_NOT_EMPTY;
    }

    return ONE_ERROR_NONE;
}

}  // namespace validation

namespace invocation {

Error error_response(const Message &message, std::function<void(void *)> callback,
                     void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::ErrorResponse params;
    const auto err = validation::error_response(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data);
    return ONE_ERROR_NONE;
}

Error soft_stop_request(const Message &message, std::function<void(void *, int)> callback,
                        void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::SoftStopRequest params;
    const auto err = validation::soft_stop_request(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, params._timeout);
    return ONE_ERROR_NONE;
}

Error allocated_request(const Message &message,
                        std::function<void(void *, Array *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::AllocatedRequest params;
    const auto err = validation::allocated_request(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, &params._data);
    return ONE_ERROR_NONE;
}

Error meta_data_request(const Message &message,
                        std::function<void(void *, Array *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::MetaDataRequest params;
    const auto err = validation::meta_data_request(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, &params._data);
    return ONE_ERROR_NONE;
}

Error live_state_request(const Message &message, std::function<void(void *)> callback,
                         void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::LiveStateRequest params;
    const auto err = validation::live_state_request(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data);
    return ONE_ERROR_NONE;
}

Error live_state_response(
    const Message &message,
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::LiveStateResponse params;
    const auto err = validation::live_state_response(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, params._players, params._max_players, params._name, params._map,
             params._mode, params._version);
    return ONE_ERROR_NONE;
}

Error host_information_request(const Message &message,
                               std::function<void(void *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::HostInformationRequest params;
    const auto err = validation::host_information_request(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data);
    return ONE_ERROR_NONE;
}

}  // namespace invocation

}  // namespace one
}  // namespace i3d