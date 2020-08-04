#include <one/arcus/arcus.h>

#include <one/arcus/internal/socket.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/message.h>

namespace one {

Server::Server() : _listen_socket(nullptr), _client_socket(nullptr), _client_connection(nullptr) {}

Server::~Server() {
    if (_listen_socket != nullptr) {
        delete _listen_socket;
        _listen_socket = nullptr;
    }

    if (_client_socket != nullptr) {
        delete _client_socket;
        _client_socket = nullptr;
    }

    if (_client_connection != nullptr) {
        delete _client_connection;
        _client_connection = nullptr;
    }
}

int Server::listen(unsigned int port) { return 0; }

int Server::update() { return 0; }

int Server::status() const { return -0; }

int Server::close() { return 0; }

Client::Client() : _socket(nullptr), _connection(nullptr) {}

Client::~Client() {
    if (_socket != nullptr) {
        delete _socket;
        _socket = nullptr;
    }

    if (_connection != nullptr) {
        delete _connection;
        _connection = nullptr;
    }
}

int Client::connect(const char* ip, unsigned int port) { return 0; }

int Client::update() { return 0; }

int Client::send_soft_stop() { return 0; }

int Client::request_server_info() { return 0; }

int Client::send(Message* message) { return 0; }

}  // namespace one
