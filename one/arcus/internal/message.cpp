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

Error live_state(const Message &message, params::LiveStateResponse &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::live_state) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_LIVE_STATE;
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

Error host_information(const Message &message, params::HostInformationResponse &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::host_information) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_HOST_INFORMATION;
    }

    const auto &payload = message.payload();
    auto err = payload.val_root_object(params._host_information);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error application_instance_information(
    const Message &message, params::ApplicationInstanceInformationResponse &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::application_instance_information) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_APPLICATION_INSTANCE_INFORMATION;
    }

    const auto &payload = message.payload();
    auto err = payload.val_root_object(params._application_instance_information);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error application_instance_status(const Message &message,
                                  params::ApplicationInstanceSetStatusRequest &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_SUPPORTED;
    }

    if (code != Opcode::application_instance_status) {
        return ONE_ERROR_MESSAGE_OPCODE_NOT_MATCHING_EXPECTING_APPLICATION_INSTANCE_STATUS;
    }

    const auto &payload = message.payload();
    const auto err = payload.val_int("status", params._status);
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

Error allocated(const Message &message, std::function<void(void *, Array *)> callback,
                void *data) {
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

Error metadata(const Message &message, std::function<void(void *, Array *)> callback,
               void *data) {
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

Error live_state(
    const Message &message,
    std::function<void(void *, int, int, const String &, const String &,
                       const String &, const String &)>
        callback,
    void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::LiveStateResponse params;
    const auto err = validation::live_state(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, params._players, params._max_players, params._name, params._map,
             params._mode, params._version);
    return ONE_ERROR_NONE;
}

Error host_information(const Message &message,
                       std::function<void(void *, Object *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::HostInformationResponse params;
    const auto err = validation::host_information(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, &params._host_information);
    return ONE_ERROR_NONE;
}

Error application_instance_information(const Message &message,
                                       std::function<void(void *, Object *)> callback,
                                       void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::ApplicationInstanceInformationResponse params;
    const auto err = validation::application_instance_information(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, &params._application_instance_information);
    return ONE_ERROR_NONE;
}

Error application_instance_status(const Message &message,
                                  std::function<void(void *, int)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_MESSAGE_CALLBACK_IS_NULLPTR;
    }

    params::ApplicationInstanceSetStatusRequest params;
    const auto err = validation::application_instance_status(message, params);
    if (is_error(err)) {
        return err;
    }

    callback(data, params._status);
    return ONE_ERROR_NONE;
}

}  // namespace invocation

}  // namespace one
}  // namespace i3d
