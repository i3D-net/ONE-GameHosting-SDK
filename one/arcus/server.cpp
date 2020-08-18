#include <one/arcus/server.h>

#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/message.h>
#include <one/arcus/opcode.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/message.h>

namespace one {

Server::Server()
    : _listen_socket(nullptr)
    , _client_socket(nullptr)
    , _client_connection(nullptr)
    , _callbacks({0}) {}

Server::~Server() {
    shutdown();
}

Error Server::init(size_t max_message_in, size_t max_message_out) {
    if (_listen_socket != nullptr || _client_socket != nullptr || _client_connection != nullptr) {
        return ONE_ERROR_SERVER_ALREADY_INITIALIZED;
    }

    auto err = init_socket_system();
    if (is_error(err)) {
        return err;
    }

    _listen_socket = new Socket();
    if (_listen_socket == nullptr) {
        return ONE_ERROR_SERVER_SOCKET_ALLOCATION_FAILED;
    }

    err = _listen_socket->init();
    if (is_error(err)) {
        shutdown();
        return err;
    }

    _client_socket = new Socket();
    if (_client_socket == nullptr) {
        shutdown();
        return ONE_ERROR_SERVER_SOCKET_ALLOCATION_FAILED;
    }

    return ONE_ERROR_NONE;
}

Error Server::shutdown() {
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
    _callbacks = {0};
    return ONE_ERROR_NONE;
}

Server::Status Server::status() const {
    if (!_client_socket->is_initialized()) {
        return Status::listening;
    }

    if (_client_connection == nullptr) {
        return Status::error;
    }

    const auto status = _client_connection->status();
    switch (status) {
        case Connection::Status::handshake_not_started:
        case Connection::Status::handshake_hello_scheduled:
        case Connection::Status::handshake_hello_sent:
            return Status::handshake;
        case Connection::Status::ready:
            return Status::ready;
        default:
            return Status::error;
    }
}

Error Server::listen(unsigned int port, int queueLength) {
    if (_listen_socket == nullptr) {
        return ONE_ERROR_SERVER_SOCKET_IS_NULLPTR;
    }

    if (!_listen_socket->is_initialized()) {
        return ONE_ERROR_SERVER_SOCKET_IS_NOT_INITIALIZED;
    }

    auto err = _listen_socket->bind(port);
    if (is_error(err)) {
        return err;
    }

    err = _listen_socket->listen(queueLength);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Server::update() {
    // Send pending outgoing messages and gather incoming messages
    // for reading.

    if (_client_socket == nullptr) {
        return ONE_ERROR_SERVER_SOCKET_IS_NULLPTR;
    }

    // Only accept new connection if client is not connected.
    if (!_client_socket->is_initialized()) {
        if (_client_connection != nullptr) {
            return ONE_ERROR_SERVER_CONNECTION_IS_NULLPTR;
        }
        bool is_ready;
        auto err = _listen_socket->ready_for_read(0.f, is_ready);
        if (is_error(err)) {
            return err;
        }
        if (!is_ready) {
            return ONE_ERROR_NONE;
        }

        std::string client_ip;
        unsigned int client_port;
        err = _listen_socket->accept(*_client_socket, client_ip, client_port);
        if (is_error(err)) {
            return ONE_ERROR_NONE;
        }

        // If no client was accepted, exit.
        if (!_client_socket->is_initialized()) {
            return ONE_ERROR_NONE;
        }

        // TODO: avoid allocation, use reset & init.
        auto connection = new Connection(*_client_socket, 1024, 1024);
        if (connection == nullptr) {
            return ONE_ERROR_SERVER_CONNECTION_IS_NULLPTR;
        }

        _client_connection = connection;

        // The Arcus Server is responsible for initiating the handshake against agents. The agent
        // waits for an initial hello packet from the Server.
        _client_connection->initiate_handshake();
        return ONE_ERROR_NONE;
    }

    auto err = _client_connection->update();
    if (is_error(err)) {
        return err;
    }

    // Read pending incoming messages.
    Message *message = nullptr;
    while (true) {
        unsigned int count = 0;
        err = _client_connection->incoming_count(count);
        if (is_error(err)) return err;
        if (count == 0) break;

        err = _client_connection->pop_incoming(&message);
        if (err != 0) {
            return err;
        }
        if (message == nullptr) {
            return ONE_ERROR_SERVER_MESSAGE_IS_NULLPTR;
        }

        err = process_incoming_message(*message);
        if (is_error(err)) {
            return err;
        }

        delete message;
    }

    return ONE_ERROR_NONE;
}

Error Server::set_soft_stop_callback(std::function<void(void *, int)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._soft_stop_request = callback;
    _callbacks._soft_stop_request_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_allocated_callback(std::function<void(void *, Array *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._allocated_request = callback;
    _callbacks._allocated_request_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_meta_data_callback(std::function<void(void *, Array *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._meta_data_request = callback;
    _callbacks._meta_data_request_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_live_state_request_callback(std::function<void(void *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._live_state_request = callback;
    _callbacks._live_state_request_data = data;
    return ONE_ERROR_NONE;
}

Error Server::send_error_response(const Message &message) {
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    return ONE_ERROR_NONE;
}

Error Server::send_live_state_response(const Message &message) {
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Server::send_host_information_request(const Message &message) {
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Server::process_incoming_message(const Message &message) {
    switch (message.code()) {
        case Opcode::soft_stop_request:
            if (_callbacks._soft_stop_request == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::soft_stop_request(message, _callbacks._soft_stop_request,
                                                 _callbacks._soft_stop_request_data);
        case Opcode::allocated_request:
            if (_callbacks._allocated_request == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::allocated_request(message, _callbacks._allocated_request,
                                                 _callbacks._allocated_request_data);
        case Opcode::meta_data_request:
            if (_callbacks._meta_data_request == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::meta_data_request(message, _callbacks._meta_data_request,
                                                 _callbacks._meta_data_request_data);
        case Opcode::live_state_request:
            if (_callbacks._live_state_request == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::live_state_request(message, _callbacks._live_state_request,
                                                  _callbacks._live_state_request_data);
        default:
            return ONE_ERROR_NONE;
    }
}

Error Server::process_outgoing_message(const Message &message) {
    Error err = ONE_ERROR_NONE;
    switch (message.code()) {
        case Opcode::live_state_response: {
            params::LifeStateResponse params;
            err = validation::live_state_response(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        case Opcode::host_information_request: {
            params::HostInformationRequest params;
            err = validation::host_information_request(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        default:
            return ONE_ERROR_NONE;
    }

    if (_client_connection == nullptr) {
        return ONE_ERROR_SERVER_CONNECTION_IS_NULLPTR;
    }

    // Todo - pointer here, passing ownership to connection.
    // error = _client_connection->push_outgoing(message);
    // if (is_error(error)) {
    //     return -1;
    // }

    return ONE_ERROR_NONE;
}

}  // namespace one
