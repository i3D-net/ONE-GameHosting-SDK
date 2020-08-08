#include <one/arcus/arcus.h>

#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/message.h>
#include <one/arcus/internal/opcodes.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/message.h>

namespace one {

Server::Server()
    : _listen_socket(nullptr)
    , _client_socket(nullptr)
    , _client_connection(nullptr)
    , _soft_stop_callback(nullptr) {}

Server::~Server() {
    shutdown();
}

int Server::init() {
    if (_listen_socket != nullptr || _client_socket != nullptr || _client_connection != nullptr) {
        return -1;
    }

    init_socket_system();

    _listen_socket = new Socket();
    if (_listen_socket == nullptr) {
        return -1;
    }

    int error = _listen_socket->init();
    if (error != 0) {
        shutdown();
        return -1;
    }

    _client_socket = new Socket();
    if (_client_socket == nullptr) {
        shutdown();
        return -1;
    }

    error = _client_socket->init();
    if (error != 0) {
        shutdown();
        return -1;
    }

    _client_connection = new Connection(*_listen_socket, 1024, 1024);
    if (_client_connection == nullptr) {
        shutdown();
        return -1;
    }

    return 0;
}

int Server::shutdown() {
    if (_client_connection != nullptr) {
        delete _client_connection;
        _client_connection = nullptr;
    }

    if (_listen_socket != nullptr) {
        delete _listen_socket;
        _listen_socket = nullptr;
    }

    if (_client_socket != nullptr) {
        delete _client_socket;
        _client_socket = nullptr;
    }

    shutdown_socket_system();

    return 0;
}

int Server::status() const {
    return 0;
}

int Server::listen(unsigned int port) {
    return 0;
}

int Server::update() {
    // Send pending outgoing messages and gather incoming messages
    // for reading.
    if (_client_connection == nullptr) {
        return -1;
    }

    int error = _client_connection->update();
    if (error != 0) {
        return error;
    }

    // Read pending incoming messages.
    Message *message = nullptr;
    while (_client_connection->incoming_count() > 0) {
        error = _client_connection->pop_incoming(&message);
        if (error != 0) {
            return -1;
        }

        if (message == nullptr) {
            return -1;
        }

        error = process_incoming_message(*message);
        if (error != 0) {
            return -1;
        }
    }

    return 0;
}

void Server::set_soft_stop_callback(std::function<void(int)> callback) {
    _soft_stop_callback = callback;
}

void Server::set_live_state_request_callback(std::function<void()> callback) {
    _live_state_request_callback = callback;
}

int Server::send_server_info(const Message &message) {
    int error = process_outgoing_message(message);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Server::process_incoming_message(const Message &message) {
    switch (message.code()) {
        case Opcodes::soft_stop:
            if (_soft_stop_callback == nullptr) {
                return 0;
            }

            return invoke::soft_stop(message, _soft_stop_callback);
        case Opcodes::live_state_request:
            if (_live_state_request_callback == nullptr) {
                return 0;
            }

            return invoke::live_state_request(message, _live_state_request_callback);
        default:
            return 0;
    }
}

int Server::process_outgoing_message(const Message &message) {
    int error = 0;
    switch (message.code()) {
        case Opcodes::live_state: {
            params::LifeState params;
            error = validate::live_state(message, params);
            if (error != 0) {
                return -1;
            }

            break;
        }
        default:
            return 0;
    }

    if (_client_connection == nullptr) {
        return -1;
    }

    error = _client_connection->push_outgoing(message);
    if (error != 0) {
        return -1;
    }

    return 0;
}

Client::Client() : _socket(nullptr), _connection(nullptr) {}

Client::~Client() {
    shutdown();
}

int Client::init() {
    init_socket_system();
    return 0;
}

int Client::shutdown() {
    if (_socket != nullptr) {
        delete _socket;
        _socket = nullptr;
    }

    if (_connection != nullptr) {
        delete _connection;
        _connection = nullptr;
    }

    shutdown_socket_system();
    return 0;
}

int Client::connect(const char *ip, unsigned int port) {
    return 0;
}

int Client::update() {
    return 0;
}

int Client::send_soft_stop() {
    return 0;
}

int Client::request_server_info() {
    return 0;
}

int Client::send(Message *message) {
    return 0;
}

}  // namespace one
