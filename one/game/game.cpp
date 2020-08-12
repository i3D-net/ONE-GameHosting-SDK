#include <one/game/game.h>

#include <one/arcus/message.h>

#include <assert.h>

namespace one {

Game::Game(unsigned int port, int queueLength, int players, int max_players,
           const std::string &name, const std::string &map, const std::string &mode,
           const std::string &version)
    : _api(nullptr)
    , _server(nullptr)
    , _port(port)
    , _queueLength(queueLength)
    , _players(players)
    , _max_players(max_players)
    , _name(name)
    , _map(map)
    , _mode(mode)
    , _version(version) {}

Game::~Game() {
    shutdown();
}

int Game::init(size_t max_message_in, size_t max_message_out) {
    if (_api != nullptr && _server != nullptr) {
        return -1;
    }

    _api = ::one_game_hosting_api();

    assert(_api != nullptr);
    assert(_api->server_api != nullptr);

    int error = _api->server_api->create(&_server, max_message_in, max_message_out);
    if (error != 0) {
        return error;
    }

    error = listen();
    if (error != 0) {
        return error;
    }

    return 0;
}

int Game::shutdown() {
    if (_server == nullptr && _api == nullptr) {
        return 0;
    }
    assert(_api != nullptr);
    assert(_api->server_api != nullptr);
    _api->server_api->destroy(&_server);
    _server = nullptr;
    _api = nullptr;
    return 0;
}

int Game::update() {
    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);
    return _api->server_api->update(_server);
}

int Game::status() const {
    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);
    return _api->server_api->status(_server);
}

int Game::send_error_response() {
    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);
    Message message;

    int error = messages::prepare_error(message);
    if (error != 0) {
        return -1;
    }

    return _api->server_api->send_error_response(_server, (OneMessagePtr)&message);
}

int Game::set_live_state_request_callback() {
    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);

    return _api->server_api->set_live_state_request_callback(_server, &send_live_state_callback,
                                                             this);
}

void Game::send_live_state_callback(void *data) {
    auto self = reinterpret_cast<Game *>(data);
    if (self == nullptr) {
        return;
    }

    assert(self->_api != nullptr && self->_api->server_api != nullptr && self->_server != nullptr);
    Message message;

    int error = messages::prepare_live_state(self->_players, self->_max_players,
                                             self->_name.c_str(), self->_map.c_str(),
                                             self->_mode.c_str(), self->_version.c_str(), message);
    if (error != 0) {
        return;
    }

    self->_api->server_api->send_live_state_response(self->_server, (OneMessagePtr)&message);
}

int Game::send_host_information_request()
{
    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);
    Message message;

    int error = messages::prepare_host_information_request(message);
    if (error != 0) {
        return -1;
    }

    return _api->server_api->send_host_information_request(_server, (OneMessagePtr)&message);
}

int Game::set_soft_stop_request_callback(void (*callback)(void *data, int timeout), void *data) {
    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);
    return _api->server_api->set_soft_stop_callback(_server, callback, data);
}

int Game::set_allocated_request_callback(void(*callback)(void*, void*), void* data)
{
    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);
    return _api->server_api->set_allocated_callback(_server, callback, data);
}

int Game::set_meta_data_request_callback(void (*callback)(void *data, void *), void *data) {
    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);
    return _api->server_api->set_meta_data_callback(_server, callback, data);
}

int Game::listen() {
    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);
    return _api->server_api->listen(_server, _port, _queueLength);
}

}  // namespace one
