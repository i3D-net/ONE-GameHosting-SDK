#include <one/arcus/client.h>

#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/message.h>
#include <one/arcus/opcode.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/message.h>

namespace one {

Client::Client() : _socket(nullptr), _connection(nullptr), _callbacks({0}) {}

Client::~Client() {
    shutdown();
}

int Client::init(size_t max_message_in, size_t max_message_out) {
    if (_socket != nullptr || _connection != nullptr) {
        return -1;
    }

    int error = init_socket_system();
    if (error != 0) {
        return -1;
    }

    _socket = new Socket();
    if (_socket == nullptr) {
        shutdown();
        return -1;
    }

    error = _socket->init();
    if (error != 0) {
        shutdown();
        return -1;
    }

    _connection = new Connection(*_socket, max_message_in, max_message_out);
    if (_connection == nullptr) {
        shutdown();
        return -1;
    }

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
    _callbacks = {0};
    return 0;
}

int Client::connect(const char *address, unsigned int port) {
    if (_socket == nullptr || _connection == nullptr) {
        return -1;
    }

    if (!_socket->is_initialized()) {
        return -1;
    }

    int error = _socket->connect(address, port);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Client::update() {
    if (_connection == nullptr) {
        return -1;
    }

    auto error = _connection->update();
    if (is_error(error)) {
        return error;
    }

    return 0;
}

Client::Status Client::status() {
    if (_connection == nullptr) {
        return Status::error;
    }

    const auto status = _connection->status();
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

int Client::send_soft_stop_request(int timeout) {
    Message message;
    messages::prepare_soft_stop_request(timeout, message);
    int error = process_outgoing_message(message);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Client::send_allocated_request(Array *data) {
    if (data == nullptr) {
        return -1;
    }

    Message message;
    messages::prepare_allocated_request(*data, message);
    int error = process_outgoing_message(message);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Client::send_meta_data_request(Array *data) {
    if (data == nullptr) {
        return -1;
    }

    Message message;
    messages::prepare_meta_data_request(*data, message);
    int error = process_outgoing_message(message);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Client::send_live_state_request() {
    Message message;
    messages::prepare_live_state_request(message);
    int error = process_outgoing_message(message);
    if (error != 0) {
        return -1;
    }

    return 0;
}

int Client::set_error_callback(std::function<void(void *)> callback, void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _callbacks._error_response = callback;
    _callbacks._error_response_data = data;
    return 0;
}

int Client::set_live_state_callback(
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _callbacks._live_state_response = callback;
    _callbacks._live_state_response_data = data;
    return 0;
}

int Client::set_host_information_request_callback(std::function<void(void *)> callback,
                                                  void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _callbacks._host_information_request = callback;
    _callbacks._host_information_request_data = data;
    return 0;
}

int Client::process_incoming_message(const Message &message) {
    switch (message.code()) {
        case Opcode::live_state_response:
            if (_callbacks._live_state_response == nullptr) {
                return 0;
            }

            return invocation::live_state_response(message, _callbacks._live_state_response,
                                                   _callbacks._live_state_response_data);
        case Opcode::host_information_request:
            if (_callbacks._host_information_request == nullptr) {
                return 0;
            }

            return invocation::host_informatio_request(message,
                                                       _callbacks._host_information_request,
                                                       _callbacks._host_information_request_data);
        default:
            return 0;
    }
}

int Client::process_outgoing_message(const Message &message) {
    int error = 0;
    switch (message.code()) {
        case Opcode::live_state_request: {
            params::LifeStateRequest params;
            error = validation::live_state_request(message, params);
            if (error != 0) {
                return -1;
            }

            break;
        }
        default:
            return 0;
    }

    if (_connection == nullptr) {
        return -1;
    }

    // Todo: message pointer here, passing ownership of memory from client to
    // connection.
    // error = _connection->push_outgoing(message);
    // if (is_error(error)) {
    //     return -1;
    // }

    return 0;
}

}  // namespace one
