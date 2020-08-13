#include <one/game/game.h>

#include <one/arcus/message.h>

#include <assert.h>

namespace one {

Game::Game(unsigned int port, int queueLength, int players, int max_players,
           const std::string &name, const std::string &map, const std::string &mode,
           const std::string &version)
    : _server(port, queueLength)
    , _players(players)
    , _max_players(max_players)
    , _name(name)
    , _map(map)
    , _mode(mode)
    , _version(version) {}

Game::~Game() {
    _server.shutdown();
}

int Game::init(size_t max_message_in, size_t max_message_out) {
    return _server.init(max_message_in, max_message_out);
}

int Game::shutdown() {
    return _server.shutdown();
}

int Game::update() {
    return _server.update();
}

int Game::status() const {
    return _server.status();
}

int Game::send_error_response() {
    return _server.send_error_response();
}

int Game::send_host_information_request() {
    return _server.send_host_information_request();
}

int Game::set_live_state_request_callback() {
    return _server.set_live_state_request_callback(&send_live_state_callback, this);
}

void Game::send_live_state_callback(void *data) {
    auto self = reinterpret_cast<Game *>(data);
    if (self == nullptr) {
        return;
    }

    self->_server.send_live_state_response(self->_players, self->_max_players, self->_name,
                                           self->_map, self->_mode, self->_version);
}

int Game::set_soft_stop_request_callback(void (*callback)(void *data, int timeout), void *data) {
    return _server.set_soft_stop_request_callback(callback, data);
}

int Game::set_allocated_request_callback(void (*callback)(void *, void *), void *data) {
    return _server.set_allocated_request_callback(callback, data);
}

int Game::set_meta_data_request_callback(void (*callback)(void *data, void *), void *data) {
    return _server.set_meta_data_request_callback(callback, data);
}

}  // namespace one
