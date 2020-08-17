#include <one/arcus/internal/message.h>

#include <one/arcus/opcode.h>
#include <one/arcus/message.h>

#include <functional>

namespace one {

namespace validation {

int error_request(const Message &message, params::ErrorResponse &) {
    const auto code = message.code();

    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcode::error_response) {
        return -1;
    }

    if (!message.payload().is_empty()) {
        return -1;
    }

    return 0;
}

int soft_stop_request(const Message &message, params::SoftStopRequest &params) {
    const auto code = message.code();

    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcode::soft_stop_request) {
        return -1;
    }

    const auto &payload = message.payload();

    const int error = payload.val_int("timeout", params._timeout);

    if (error != 0) {
        return error;
    }

    return 0;
}

int allocated_request(const Message &message, params::AllocatedRequest &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcode::allocated_request) {
        return -1;
    }

    const auto &payload = message.payload();

    int error = payload.val_array("data", params._data);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int meta_data_request(const Message &message, params::MetaDataRequest &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcode::meta_data_request) {
        return -1;
    }

    const auto &payload = message.payload();

    int error = payload.val_array("data", params._data);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int live_state_request(const Message &message, params::LifeStateRequest &) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcode::live_state_request) {
        return -1;
    }

    if (!message.payload().is_empty()) {
        return -1;
    }

    return 0;
}

int live_state_response(const Message &message, params::LifeStateResponse &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcode::live_state_response) {
        return -1;
    }

    const auto &payload = message.payload();

    int error = payload.val_int("players", params._players);
    if (error != 0) {
        return error;
    }

    error = payload.val_int("maxPlayers", params._max_players);
    if (error != 0) {
        return error;
    }

    error = payload.val_string("name", params._name);
    if (error != 0) {
        return error;
    }

    error = payload.val_string("map", params._map);
    if (error != 0) {
        return error;
    }

    error = payload.val_string("mode", params._mode);
    if (error != 0) {
        return error;
    }

    error = payload.val_string("version", params._version);
    if (error != 0) {
        return error;
    }

    return 0;
}

int host_information_request(const Message &message, params::HostInformationRequest &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcode::host_information_request) {
        return -1;
    }

    if (!message.payload().is_empty()) {
        return -1;
    }

    return 0;
}

}  // namespace validation

namespace invocation {

int error_response(const Message &message, std::function<void(void *)> callback, void *data) {
    if (callback == nullptr) {
        return -1;
    }

    params::ErrorResponse params;
    const int error = validation::error_request(message, params);
    if (error != 0) {
        return error;
    }

    callback(data);
    return 0;
}

int soft_stop_request(const Message &message, std::function<void(void *, int)> callback,
                      void *data) {
    if (callback == nullptr) {
        return -1;
    }

    params::SoftStopRequest params;
    const int error = validation::soft_stop_request(message, params);
    if (error != 0) {
        return error;
    }

    callback(data, params._timeout);
    return 0;
}

int allocated_request(const Message &message, std::function<void(void *, Array *)> callback,
                      void *data) {
    if (callback == nullptr) {
        return -1;
    }

    params::AllocatedRequest params;
    const int error = validation::allocated_request(message, params);
    if (error != 0) {
        return error;
    }

    callback(data, &params._data);
    return 0;
}

int meta_data_request(const Message &message, std::function<void(void *, Array *)> callback,
                      void *data) {
    if (callback == nullptr) {
        return -1;
    }

    params::MetaDataRequest params;
    const int error = validation::meta_data_request(message, params);
    if (error != 0) {
        return error;
    }

    callback(data, &params._data);
    return 0;
}

int live_state_request(const Message &message, std::function<void(void *)> callback, void *data) {
    if (callback == nullptr) {
        return -1;
    }

    params::LifeStateRequest params;
    const int error = validation::live_state_request(message, params);
    if (error != 0) {
        return error;
    }

    callback(data);
    return 0;
}

int live_state_response(
    const Message &message,
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data) {
    if (callback == nullptr) {
        return -1;
    }

    params::LifeStateResponse params;
    const int error = validation::live_state_response(message, params);
    if (error != 0) {
        return error;
    }

    callback(data, params._players, params._max_players, params._name, params._map, params._mode,
             params._version);
    return 0;
}

int host_informatio_request(const Message &message, std::function<void(void *)> callback,
                            void *data) {
    if (callback == nullptr) {
        return -1;
    }

    params::HostInformationRequest params;
    const int error = validation::host_information_request(message, params);
    if (error != 0) {
        return error;
    }

    callback(data);
    return 0;
}

}  // namespace invocation

}  // namespace one
