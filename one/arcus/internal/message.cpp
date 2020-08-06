#include <one/arcus/internal/message.h>

#include <one/arcus/internal/opcodes.h>
#include <one/arcus/message.h>

#include <functional>

namespace one {

namespace validate {

int soft_stop(const Message &message, params::SoftStop &params) {
    const auto code = message.code();

    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcodes::soft_stop) {
        return -1;
    }

    const auto &payload = message.payload();

    const int error = payload.val_int("timeout", params._timeout);

    if (error != 0) {
        return error;
    }

    return 0;
}

int live_state_request(const Message &message, params::LifeStateRequest &) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcodes::live_state_request) {
        return -1;
    }

    if (!message.payload().is_empty()) {
        return -1;
    }

    return 0;
}

int live_state(const Message &message, params::LifeState &params) {
    const auto code = message.code();
    if (!is_opcode_supported(code)) {
        return -1;
    }

    if (code != Opcodes::live_state) {
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

}  // namespace validate

namespace invoke {

int soft_stop(const Message &message, std::function<void(int)> callback) {
    if (callback == nullptr) {
        return -1;
    }

    params::SoftStop params;
    const int error = validate::soft_stop(message, params);
    if (error != 0) {
        return error;
    }

    callback(params._timeout);
    return 0;
}

int live_state_request(const Message &message, std::function<void()> callback) {
    if (callback == nullptr) {
        return -1;
    }

    params::LifeStateRequest params;
    const int error = validate::live_state_request(message, params);
    if (error != 0) {
        return error;
    }

    callback();
    return 0;
}

}  // namespace invoke

}  // namespace one
