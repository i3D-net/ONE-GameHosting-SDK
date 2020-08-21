#include <one/game/one_server_wrapper.h>

#include <assert.h>

#include <one/arcus/c_api.h>

namespace game {

OneServerWrapper::OneServerWrapper(unsigned int port)
    : _server(nullptr)
    , _error(nullptr)
    , _live_state(nullptr)
    , _host_information(nullptr)
    , _port(port)
    , _status(Status::none)
    , _game_state() {}

OneServerWrapper::~OneServerWrapper() {
    shutdown();
}

void OneServerWrapper::init() {
    if (_server != nullptr) {
        return;
    }

    // Create the one server. This example has a game server with a
    // corresponding arcus server. Multiple arcus servers may be created if
    // the process is hosting multiple game servers. Each game server must have
    // one corresponding arcus server.

    OneError error = one_server_create(&_server);
    if (error != 0) {
        // Todo: log error.
        return;
    }

    // Create cached messages that can be sent as responses.

    error = one_message_create(&_error);
    if (error != 0) {
        // Todo: log error.
        return;
    }

    error = one_message_create(&_live_state);
    if (error != 0) {
        // Todo: log error.
        return;
    }

    error = one_message_create(&_host_information);
    if (error != 0) {
        // Todo: log error.
        return;
    }

    //---------------
    // Set callbacks.

    error = one_server_set_soft_stop_callback(_server, soft_stop, this);

    //------------------
    // Start the server.

    // Todo - retry listen loop in update if listen fails...

    error = one_server_listen(_server, _port);
    if (error != 0) {
        // Todo: log error.
        return;
    }

    _status = Status::active;
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

    _status = Status::none;
}

void OneServerWrapper::set_game_state(const GameState &state) {
    _game_state = state;
}

void OneServerWrapper::update() {
    assert(_server != nullptr);
    OneError error = one_server_update(_server);
    if (is_error(error)) {
        // Todo: log.
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

// OneError OneServerWrapper::send_live_state_response(int player, int max_player, const std::string &name,
//                                                const std::string &map, const std::string &mode,
//                                                const std::string &version) {
//     assert(_server != nullptr && _error != nullptr);

//     int error = one_message_prepare_live_state_response(
//         player, max_player, name.c_str(), map.c_str(), mode.c_str(), version.c_str(), _live_state);
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
        // Todo: log.
        return;
    }
    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    if (wrapper->_soft_stop_callback) {
        wrapper->_soft_stop_callback(timeout_seconds);
    }
}

}  // namespace game
