#include <one/game/game.h>

#include <assert.h>

namespace one {

Game::Game(unsigned int port) : _api(nullptr), _server(nullptr), _port(port), _number_players(0) {}

Game::~Game() {
    deinit();
}

int Game::init() {
    if (_api != nullptr && _server != nullptr) {
        return -1;
    }

    _api = ::one_game_hosting_api();

    assert(_api != nullptr);
    assert(_api->server_api != nullptr);

    int error = _api->server_api->create(&_server);
    if (error != 0) {
        return error;
    }

    error = listen();
    if (error != 0) {
        return error;
    }

    return 0;
}

int Game::deinit() {
    if (_server == nullptr && _api == nullptr) {
        return 0;
    }

    assert(_api != nullptr && _api->server_api != nullptr && _server != nullptr);

    int error = close();
    _api->server_api->destroy(&_server);
    _api = nullptr;

    return error;
}

int Game::tick() {
    int error = update_number_players();
    if (error != 0) {
        return error;
    }

    error = handle_updates();
    if (error != 0) {
        return error;
    }

    return 0;
}

int Game::update_number_players() {
    ++_number_players;
    return 0;
}

int Game::handle_updates() {
    int error = update();
    if (error != 0) {
        return error;
    }

    // TODO: handle any incoming/outgoing updates.

    return 0;
}

int Game::update() {
    return _api->server_api->update(_server);
}

int Game::status() const {
    return _api->server_api->status(_server);
}

int Game::listen() {
    return _api->server_api->listen(_server, _port);
}

int Game::close() {
    return _api->server_api->close(_server);
}

}  // namespace one
