#include <one/arcus/client.h>

#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/message.h>
#include <one/arcus/opcode.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/message.h>

namespace one {

constexpr size_t connection_retry_delay_seconds = 5;

Client::Client()
    : _socket(nullptr)
    , _connection(nullptr)
    , _is_connected(false)
    , _last_connection_attempt_time(steady_clock::duration::zero())
    , _callbacks({0}) {}

Client::~Client() {
    shutdown();
}

int Client::init(const char *address, unsigned int port) {
    _server_address = address;
    _server_port = port;

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

    _connection =
        new Connection(Connection::max_message_default, Connection::max_message_default);
    _connection->init(*_socket);
    if (_connection == nullptr) {
        shutdown();
        return -1;
    }

    return 0;
}

int Client::shutdown() {
    _is_connected = false;

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

Error Client::connect() {
    if (!is_initialized()) {
        return ONE_ERROR_CLIENT_NOT_INITIALIZED;
    }
    assert(_socket != nullptr);
    assert(_connection != nullptr);

    if (!_socket->is_initialized()) {
        return ONE_ERROR_CLIENT_NOT_INITIALIZED;
    }

    Error error = _socket->connect(_server_address.c_str(), _server_port);
    if (error != 0) {
        return error;
    }
    _is_connected = true;

    return ONE_ERROR_NONE;
}

int Client::update() {
    if (!is_initialized()) {
        return ONE_ERROR_CLIENT_NOT_INITIALIZED;
    }
    assert(_connection != nullptr);

    // If not connected, attempt to connect at an interval.
    if (!_is_connected) {
        const auto now = steady_clock::now();
        const size_t delta =
            duration_cast<seconds>(now - _last_connection_attempt_time).count();
        if (delta > connection_retry_delay_seconds) {
            _last_connection_attempt_time = now;
            auto error = connect();
            // If connection fails, then nothing else to update. Return the error.
            if (is_error(error)) {
                return error;
            }
        }
    }

    auto error = _connection->update();
    // In the case of any error, reset the socket for reconnection attempt.
    if (is_error(error)) {
        _connection->shutdown();
        _socket->close();
        _is_connected = false;
        _socket->init();
        _connection->init(*_socket);
        return error;
    }

    return ONE_ERROR_NONE;
}

Client::Status Client::status() {
    if (!is_initialized()) {
        return Status::uninitialized;
    }
    if (!_is_connected) {
        return Status::connecting;
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

            return invocation::live_state_response(message,
                                                   _callbacks._live_state_response,
                                                   _callbacks._live_state_response_data);
        case Opcode::host_information_request:
            if (_callbacks._host_information_request == nullptr) {
                return 0;
            }

            return invocation::host_information_request(
                message, _callbacks._host_information_request,
                _callbacks._host_information_request_data);
        default:
            return 0;
    }
}

int Client::process_outgoing_message(const Message &message) {
    int error = 0;
    switch (message.code()) {
        case Opcode::live_state_request: {
            params::LiveStateRequest params;
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
