#include <one/game/one_server_wrapper.h>

#include <one/arcus/c_api.h>
#include <one/arcus/c_error.h>

#include <one/game/log.h>

#include <assert.h>

namespace game {

OneServerWrapper::OneServerWrapper(unsigned int port)
    : _server(nullptr)
    , _error(nullptr)
    , _live_state(nullptr)
    , _host_information(nullptr)
    , _port(port)
    , _game_state() {}

OneServerWrapper::~OneServerWrapper() {
    shutdown();
}

void OneServerWrapper::init() {
    if (_server != nullptr || _error != nullptr || _live_state != nullptr ||
        _host_information != nullptr) {
        L_ERROR("already init");
        return;
    }

    // Create the one server. This example has a game server with a
    // corresponding arcus server. Multiple arcus servers may be created if
    // the process is hosting multiple game servers. Each game server must have
    // one corresponding arcus server.

    OneError err = one_server_create(&_server);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    // Create cached messages that can be sent as responses.

    err = one_message_create(&_error);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_message_create(&_live_state);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_message_create(&_host_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    //---------------
    // Set callbacks.

    err = one_server_set_soft_stop_callback(_server, soft_stop, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    //------------------
    // Start the server.

    // Todo - retry listen loop in update if listen fails...

    err = one_server_listen(_server, _port);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    L_INFO("init done");
}

void OneServerWrapper::shutdown() {
    if (_server == nullptr && _error == nullptr && _live_state == nullptr &&
        _host_information == nullptr) {
        return;
    }

    one_server_destroy(&_server);
    one_message_destroy(&_error);
    one_message_destroy(&_live_state);
    one_message_destroy(&_host_information);
}

void OneServerWrapper::set_game_state(const GameState &state) {
    _game_state = state;
}

void OneServerWrapper::update() {
    assert(_server != nullptr);
    OneError err = one_server_update(_server);
    if (is_error(err)) {
        L_ERROR(error_text(err));
    }
}

OneServerWrapper::Status OneServerWrapper::status() const {
    int status = 0;
    OneError err = one_server_status(_server, &status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return Status::unknown;
    }
    switch (status) {
        case ONE_SERVER_STATUS_UNINITIALIZED:
            return Status::uninitialized;
        case ONE_SERVER_STATUS_INITIALIZED:
            return Status::initialized;
        case ONE_SERVER_STATUS_WAITING_FOR_CLIENT:
            return Status::waiting_for_client;
        case ONE_SERVER_STATUS_HANDSHAKE:
            return Status::handshake;
        case ONE_SERVER_STATUS_READY:
            return Status::ready;
        case ONE_SERVER_STATUS_ERROR:
            return Status::error;
        default:
            return Status::unknown;
    }
}

// OneError OneServerWrapper::send_error_response() {
//     assert(_server != nullptr && _error != nullptr);

//     int error = one_message_prepare_error_response(_error);
//     if (error != 0) {
//         return -1;
//     }

//     return one_server_send_error_response(_server, _error);
// }

// OneError OneServerWrapper::send_live_state_response(int player, int max_player,
// const std::string &name,
//                                                const std::string &map, const
//                                                std::string &mode, const std::string
//                                                &version) {
//     assert(_server != nullptr && _error != nullptr);

//     int error = one_message_prepare_live_state_response(
//         player, max_player, name.c_str(), map.c_str(), mode.c_str(),
//         version.c_str(), _live_state);
//     if (error != 0) {
//         return -1;
//     }

//     return one_server_send_live_state_response(_server, _live_state);
// }

// OneError OneServerWrapper::send_host_information_request() {
//     assert(_server != nullptr && _error != nullptr);

//     int error = one_message_prepare_host_information_request(_host_information);
//     if (error != 0) {
//         return -1;
//     }

//     return one_server_send_host_information_request(_server, _error);
// }

void OneServerWrapper::set_soft_stop_callback(std::function<void(int)> callback) {
    _soft_stop_callback = callback;
}

// Tell the server to shutdown at the next appropriate time for its users (e.g.,
// after a match end).
void OneServerWrapper::soft_stop(void *userdata, int timeout_seconds) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }
    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    if (wrapper->_soft_stop_callback) {
        wrapper->_soft_stop_callback(timeout_seconds);
    }
}

}  // namespace game
