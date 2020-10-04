#include <one/arcus/internal/message.h>

#include <one/arcus/opcode.h>
#include <one/arcus/message.h>

#include <functional>

namespace i3d {
namespace one {

namespace validation {

Error soft_stop(const Message &message, params::SoftStopRequest &params) {
    const auto code = message.code();

    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::soft_stop) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_SOFT_STOP;
    }

    const auto &payload = message.payload();
    const auto err = payload.val_int("timeout", params._timeout);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error allocated(const Message &message, params::AllocatedRequest &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::allocated) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_ALLOCATED;
    }

    const auto &payload = message.payload();
    auto err = payload.val_array("data", params._data);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error metadata(const Message &message, params::MetaDataRequest &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::metadata) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_METADATA;
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

    const auto &payload = message.payload();
    if (!payload.is_empty()) {
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

    const auto &payload = message.payload();
    if (!payload.is_empty()) {
        return ONE_ERROR_MESSAGE_OPCODE_PAYLOAD_NOT_EMPTY;
    }

    return ONE_ERROR_NONE;
}

Error host_information_response(const Message &message,
                                params::HostInformationResponse &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::host_information_response) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_HOST_INFORMATION_RESPONSE;
    }

    const auto &payload = message.payload();
    auto err = payload.val_root_object(params._host_information);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error application_instance_information_request(
    const Message &message, params::ApplicationInstanceInformationRequest &) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::application_instance_information_request) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_APPLICATION_INSTANCE_INFORMATION_REQUEST;
    }

    const auto &payload = message.payload();
    if (!payload.is_empty()) {
        return ONE_ERROR_MESSAGE_OPCODE_PAYLOAD_NOT_EMPTY;
    }

    return ONE_ERROR_NONE;
}

Error application_instance_information_response(
    const Message &message, params::ApplicationInstanceInformationResponse &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::application_instance_information_response) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_APPLICATION_INSTANCE_INFORMATION_RESPONSE;
    }

    const auto &payload = message.payload();
    auto err = payload.val_root_object(params._application_instance_information);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error application_instance_get_status_request(
    const Message &message, params::ApplicationInstanceGetStatusRequest &) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::application_instance_get_status_request) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_APPLICATION_INSTANCE_GET_STATUS_REQUEST;
    }

    const auto &payload = message.payload();
    if (!payload.is_empty()) {
        return ONE_ERROR_MESSAGE_OPCODE_PAYLOAD_NOT_EMPTY;
    }

    return ONE_ERROR_NONE;
}

Error application_instance_get_status_response(
    const Message &message, params::ApplicationInstanceGetStatusResponse &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::application_instance_get_status_response) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_APPLICATION_INSTANCE_GET_STATUS_RESPONSE;
    }

    const auto &payload = message.payload();
    const auto err = payload.val_int("status", params._status);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error application_instance_set_status_request(
    const Message &message, params::ApplicationInstanceSetStatusRequest &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::application_instance_set_status_request) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_APPLICATION_INSTANCE_SET_STATUS_REQUEST;
    }

    const auto &payload = message.payload();
    const auto err = payload.val_int("status", params._status);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error application_instance_set_status_response(
    const Message &message, params::ApplicationInstanceSetStatusResponse &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::application_instance_set_status_response) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_APPLICATION_INSTANCE_SET_STATUS_RESPONSE;
    }

    const auto &payload = message.payload();
    const auto err = payload.val_int("code", params._code);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

}  // namespace validation

namespace invocation {

Error soft_stop(const Message &message, std::function<void(void *, int)> callback,
                        void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::SoftStopRequest params;
    const auto err = validation::soft_stop(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, params._timeout);
    return ONE_ERROR_NONE;
}

Error allocated(const Message &message,
                        std::function<void(void *, Array *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::AllocatedRequest params;
    const auto err = validation::allocated(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, &params._data);
    return ONE_ERROR_NONE;
}

Error metadata(const Message &message,
                        std::function<void(void *, Array *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::MetaDataRequest params;
    const auto err = validation::metadata(message, params);
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

Error host_information_response(const Message &message,
                                std::function<void(void *, Object *)> callback,
                                void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::HostInformationResponse params;
    const auto err = validation::host_information_response(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, &params._host_information);
    return ONE_ERROR_NONE;
}

Error application_instance_information_request(const Message &message,
                                               std::function<void(void *)> callback,
                                               void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::ApplicationInstanceInformationRequest params;
    const auto err =
        validation::application_instance_information_request(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data);
    return ONE_ERROR_NONE;
}

Error application_instance_information_response(
    const Message &message, std::function<void(void *, Object *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::ApplicationInstanceInformationResponse params;
    const auto err =
        validation::application_instance_information_response(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, &params._application_instance_information);
    return ONE_ERROR_NONE;
}

Error application_instance_get_status_request(const Message &message,
                                              std::function<void(void *)> callback,
                                              void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::ApplicationInstanceGetStatusRequest params;
    const auto err = validation::application_instance_get_status_request(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data);
    return ONE_ERROR_NONE;
}

Error application_instance_get_status_response(const Message &message,
                                               std::function<void(void *, int)> callback,
                                               void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::ApplicationInstanceGetStatusResponse params;
    const auto err =
        validation::application_instance_get_status_response(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, params._status);
    return ONE_ERROR_NONE;
}

Error application_instance_set_status_request(const Message &message,
                                              std::function<void(void *, int)> callback,
                                              void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::ApplicationInstanceSetStatusRequest params;
    const auto err = validation::application_instance_set_status_request(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, params._status);
    return ONE_ERROR_NONE;
}

Error application_instance_set_status_response(const Message &message,
                                               std::function<void(void *, int)> callback,
                                               void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::ApplicationInstanceSetStatusResponse params;
    const auto err =
        validation::application_instance_set_status_response(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, params._code);
    return ONE_ERROR_NONE;
}

}  // namespace invocation

}  // namespace one
}  // namespace i3d
