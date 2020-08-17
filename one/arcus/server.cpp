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

int Server::init(size_t max_message_in, size_t max_message_out) {
    if (_listen_socket != nullptr || _client_socket != nullptr || _client_connection != nullptr) {
        return -1;
    }

    int error = init_socket_system();
    if (error != 0) {
        return -1;
    }

    _listen_socket = new Socket();
    if (_listen_socket == nullptr) {
        return -1;
    }

    error = _listen_socket->init();
    if (error != 0) {
        shutdown();
        return -1;
    }

    _client_socket = new Socket();
    if (_client_socket == nullptr) {
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
    _callbacks = {0};
    return 0;
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

int Server::listen(unsigned int port, int queueLength) {
    if (_listen_socket == nullptr) {
        return -1;
    }

    if (!_listen_socket->is_initialized()) {
        return -1;
    }

    int error = _listen_socket->bind(port);
    if (error != 0) {
        return -1;
    }

    error = _listen_socket->listen(queueLength);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Server::update() {
    // Send pending outgoing messages and gather incoming messages
    // for reading.

    if (_client_socket == nullptr) {
        return -1;
    }

    // Only accept new connection if client is not connected.
    if (!_client_socket->is_initialized()) {
        if (_client_connection != nullptr) {
            return -1;
        }
        bool is_ready;
        const auto err = _listen_socket->ready_for_read(0.f, is_ready);
        if (is_error(err)) {
            return (int)err;
        }
        if (!is_ready) {
            return 0;
        }

        std::string client_ip;
        unsigned int client_port;
        int error = _listen_socket->accept(*_client_socket, client_ip, client_port);
        if (error != 0) {
            return 0;
        }

        // If no client was accepted, exit.
        if (!_client_socket->is_initialized()) {
            return 0;
        }

        // TODO: avoid allocation, use reset & init.
        auto connection = new Connection(*_client_socket, 1024, 1024);
        if (connection == nullptr) {
            return -1;
        }

        _client_connection = connection;

        // The Arcus Server is responsible for initiating the handshake against agents. The agent
        // waits for an initial hello packet from the Server.
        _client_connection->initiate_handshake();
        return 0;
    }

    auto error = _client_connection->update();
    if (is_error(error)) {
        return error;
    }

    // Read pending incoming messages.
    Message *message = nullptr;
    while (true) {
        unsigned int count = 0;
        error = _client_connection->incoming_count(count);
        if (is_error(error)) return -1;
        if (count == 0) break;

        error = _client_connection->pop_incoming(&message);
        if (error != 0) {
            return -1;
        }
        if (message == nullptr) {
            return -1;
        }

        error = (Error)process_incoming_message(*message);
        if (error != 0) {
            return -1;
        }

        delete message;
    }

    return 0;
}

int Server::set_soft_stop_callback(std::function<void(void *, int)> callback, void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _callbacks._soft_stop_request = callback;
    _callbacks._soft_stop_request_data = data;
    return 0;
}

int Server::set_allocated_callback(std::function<void(void *, Array *)> callback, void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _callbacks._allocated_request = callback;
    _callbacks._allocated_request_data = data;
    return 0;
}

int Server::set_meta_data_callback(std::function<void(void *, Array *)> callback, void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _callbacks._meta_data_request = callback;
    _callbacks._meta_data_request_data = data;
    return 0;
}

int Server::set_live_state_request_callback(std::function<void(void *)> callback, void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _callbacks._live_state_request = callback;
    _callbacks._live_state_request_data = data;
    return 0;
}

int Server::send_error_response(const Message &message) {
    int error = process_outgoing_message(message);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Server::send_live_state_response(const Message &message) {
    int error = process_outgoing_message(message);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Server::send_host_information_request(const Message &message) {
    int error = process_outgoing_message(message);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Server::process_incoming_message(const Message &message) {
    switch (message.code()) {
        case Opcode::soft_stop_request:
            if (_callbacks._soft_stop_request == nullptr) {
                return 0;
            }

            return invocation::soft_stop_request(message, _callbacks._soft_stop_request,
                                                 _callbacks._soft_stop_request_data);
        case Opcode::allocated_request:
            if (_callbacks._allocated_request == nullptr) {
                return 0;
            }

            return invocation::allocated_request(message, _callbacks._allocated_request,
                                                 _callbacks._allocated_request_data);
        case Opcode::meta_data_request:
            if (_callbacks._meta_data_request == nullptr) {
                return 0;
            }

            return invocation::meta_data_request(message, _callbacks._meta_data_request,
                                                 _callbacks._meta_data_request_data);
        case Opcode::live_state_request:
            if (_callbacks._live_state_request == nullptr) {
                return 0;
            }

            return invocation::live_state_request(message, _callbacks._live_state_request,
                                                  _callbacks._live_state_request_data);
        default:
            return 0;
    }
}

int Server::process_outgoing_message(const Message &message) {
    int error = 0;
    switch (message.code()) {
        case Opcode::live_state_response: {
            params::LifeStateResponse params;
            error = validation::live_state_response(message, params);
            if (error != 0) {
                return -1;
            }

            break;
        }
        case Opcode::host_information_request: {
            params::HostInformationRequest params;
            error = validation::host_information_request(message, params);
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

    // Todo - pointer here, passing ownership to connection.
    // error = _client_connection->push_outgoing(message);
    // if (is_error(error)) {
    //     return -1;
    // }

    return 0;
}

}  // namespace one
