#include <one/game/one_server_wrapper.h>

#include <assert.h>

namespace one {

OneServerWrapper::OneServerWrapper(unsigned int port, int queueLength)
    : _server(nullptr)
    , _error(nullptr)
    , _live_state(nullptr)
    , _host_information(nullptr)
    , _port(port)
    , _queueLength(queueLength) {}

OneServerWrapper::~OneServerWrapper() {
    shutdown();
}

int OneServerWrapper::init(size_t max_message_in, size_t max_message_out) {
    if (_server != nullptr) {
        return -1;
    }

    int error = one_server_create(max_message_in, max_message_out, &_server);
    if (error != 0) {
        return error;
    }

    error = one_message_create(&_error);
    if (error != 0) {
        return error;
    }

    error = one_message_create(&_live_state);
    if (error != 0) {
        return error;
    }

    error = one_message_create(&_host_information);
    if (error != 0) {
        return error;
    }

    error = listen();
    if (error != 0) {
        return error;
    }

    return 0;
}

int OneServerWrapper::shutdown() {
    if (_server == nullptr && _error == nullptr && _live_state == nullptr &&
        _host_information == nullptr) {
        return 0;
    }

    one_server_destroy(&_server);
    one_message_destroy(&_error);
    one_message_destroy(&_live_state);
    one_message_destroy(&_host_information);
    return 0;
}

int OneServerWrapper::update() {
    assert(_server != nullptr);
    return one_server_update(_server);
}

int OneServerWrapper::status() const {
    assert(_server != nullptr);
    return one_server_status(_server);
}

int OneServerWrapper::send_error_response() {
    assert(_server != nullptr && _error != nullptr);

    int error = one_message_prepare_error_response(_error);
    if (error != 0) {
        return -1;
    }

    return one_server_send_error_response(_server, _error);
}

int OneServerWrapper::send_live_state_response(int player, int max_player, const std::string &name,
                                               const std::string &map, const std::string &mode,
                                               const std::string &version) {
    assert(_server != nullptr && _error != nullptr);

    int error = one_message_prepare_live_state_response(
        player, max_player, name.c_str(), map.c_str(), mode.c_str(), version.c_str(), _live_state);
    if (error != 0) {
        return -1;
    }

    return one_server_send_live_state_response(_server, _live_state);
}

int OneServerWrapper::send_host_information_request() {
    assert(_server != nullptr && _error != nullptr);

    int error = one_message_prepare_host_information_request(_host_information);
    if (error != 0) {
        return -1;
    }

    return one_server_send_host_information_request(_server, _error);
}

int OneServerWrapper::set_soft_stop_request_callback(void (*callback)(void *data, int timeout),
                                                     void *data) {
    assert(_server != nullptr);
    return one_server_set_soft_stop_callback(_server, callback, data);
}

int OneServerWrapper::set_allocated_request_callback(void (*callback)(void *, void *), void *data) {
    assert(_server != nullptr);
    return one_server_set_allocated_callback(_server, callback, data);
}

int OneServerWrapper::set_meta_data_request_callback(void (*callback)(void *data, void *),
                                                     void *data) {
    assert(_server != nullptr);
    return one_server_set_meta_data_callback(_server, callback, data);
}

int OneServerWrapper::set_live_state_request_callback(void (*callback)(void *), void *data) {
    assert(_server != nullptr);
    return one_server_set_live_state_request_callback(_server, callback, data);
}

int OneServerWrapper::prepare_error_response(OneMessagePtr message) {
    assert(message != nullptr);
    return one_message_prepare_error_response(message);
}

int OneServerWrapper::prepare_live_state_response(int player, int max_player, const char *name,
                                                  const char *map, const char *mode,
                                                  const char *version, OneMessagePtr message) {
    assert(message != nullptr);
    return one_message_prepare_live_state_response(player, max_player, name, map, mode, version,
                                                   message);
}

int OneServerWrapper::prepare_host_information_request(OneMessagePtr message) {
    assert(message != nullptr);
    return one_message_prepare_host_information_request(message);
}

int OneServerWrapper::listen() {
    assert(_server != nullptr);
    return one_server_listen(_server, _port, _queueLength);
}

}  // namespace one
