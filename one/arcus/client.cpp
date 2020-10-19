#include <one/arcus/client.h>

#include <one/arcus/allocator.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/message.h>
#include <one/arcus/opcode.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/message.h>

//#define ONE_ARCUS_CLIENT_LOGGING

#ifdef ONE_ARCUS_CLIENT_LOGGING
    #include <iostream>
#endif

namespace i3d {
namespace one {

constexpr size_t connection_retry_delay_seconds = 5;

Client::Client()
    : _server_address("")
    , _server_port(0)
    , _socket(nullptr)
    , _connection(nullptr)
    , _is_connected(false)
    , _last_connection_attempt_time(steady_clock::duration::zero())
    , _callbacks({0}) {}

Client::~Client() {
    shutdown();
}

Error Client::init(const char *address, unsigned int port) {
    const std::lock_guard<std::mutex> lock(_client);

    _server_address = address;
    _server_port = port;

    if (_socket != nullptr) {
        return ONE_ERROR_VALIDATION_SOCKET_IS_NULLPTR;
    }

    if (_connection != nullptr) {
        return ONE_ERROR_VALIDATION_CONNECTION_IS_NULLPTR;
    }

    auto err = init_socket_system();
    if (is_error(err)) {
        return err;
    }

    _socket = allocator::create<Socket>();
    if (_socket == nullptr) {
        shutdown();
        return ONE_ERROR_VALIDATION_SOCKET_IS_NULLPTR;
    }

    err = _socket->init();
    if (is_error(err)) {
        shutdown();
        return err;
    }

    const auto max_incoming = Connection::max_message_default;
    const auto max_outgoing = Connection::max_message_default;
    _connection = allocator::create<Connection>(max_incoming, max_outgoing);
    _connection->init(*_socket);
    if (_connection == nullptr) {
        shutdown();
        return ONE_ERROR_VALIDATION_CONNECTION_IS_NULLPTR;
    }

    return ONE_ERROR_NONE;
}

void Client::shutdown() {
    const std::lock_guard<std::mutex> lock(_client);

    _is_connected = false;

    if (_socket != nullptr) {
        allocator::destroy<Socket>(_socket);
        _socket = nullptr;
    }

    if (_connection != nullptr) {
        allocator::destroy<Connection>(_connection);
        _connection = nullptr;
    }

    shutdown_socket_system();
    _callbacks = {0};
}

Error Client::update() {
    const std::lock_guard<std::mutex> lock(_client);

    if (!is_initialized()) {
        return ONE_ERROR_CLIENT_NOT_INITIALIZED;
    }
    assert(_connection != nullptr);
    assert(_socket != nullptr);

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
        } else {
            // Not connected, wait until next connection attempt.
            return ONE_ERROR_NONE;
        }
    }

    auto close_client = [this](const Error passthrough_err) -> Error {
#ifdef ONE_ARCUS_CLIENT_LOGGING
        String ip;
        unsigned int port;
        _socket->address(ip, port);
        std::cout << "ip: " << ip << ", port: " << port << ", closing client"
                  << std::endl;
#endif

        _connection->shutdown();
        _socket->close();
        _is_connected = false;
        _last_connection_attempt_time =
            steady_clock::time_point(steady_clock::duration::zero());
        _socket->init();
        _connection->init(*_socket);
        return passthrough_err;
    };

    auto err = _connection->update();
    // In the case of any error, reset the socket for reconnection attempt.
    if (is_error(err)) {
        return close_client(err);
    }

    // Read pending incoming messages.
    while (true) {
        unsigned int count = 0;
        err = _connection->incoming_count(count);
        if (is_error(err)) return close_client(err);
        if (count == 0) break;

        err = _connection->remove_incoming(
            [this](const Message &message) { return process_incoming_message(message); });
        if (is_error(err)) return close_client(err);
    }

    return ONE_ERROR_NONE;
}

String Client::status_to_string(Status status) {
    switch (status) {
        case Status::uninitialized:
            return "uninitialized";
        case Status::connecting:
            return "connecting";
        case Status::handshake:
            return "handshake";
        case Status::ready:
            return "ready";
        case Status::error:
            return "error";
    }
    return "unknown";
}

Client::Status Client::status() const {
    const std::lock_guard<std::mutex> lock(_client);

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

Error Client::send_soft_stop(int timeout) {
    const std::lock_guard<std::mutex> lock(_client);

    Message message;
    messages::prepare_soft_stop(timeout, message);
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Client::send_allocated(Array &data) {
    const std::lock_guard<std::mutex> lock(_client);

    Message message;
    messages::prepare_allocated(data, message);
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Client::send_metadata(Array &data) {
    const std::lock_guard<std::mutex> lock(_client);

    Message message;
    messages::prepare_metadata(data, message);
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Client::send_host_information(Object &data) {
    const std::lock_guard<std::mutex> lock(_client);

    Message message;
    messages::prepare_host_information(data, message);
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Client::send_application_instance_information(Object &data) {
    const std::lock_guard<std::mutex> lock(_client);

    Message message;
    messages::prepare_application_instance_information(data, message);
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Client::set_live_state_callback(
    std::function<void(void *, int, int, const String &, const String &,
                       const String &, const String &)>
        callback,
    void *userdata) {
    const std::lock_guard<std::mutex> lock(_client);

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    _callbacks._live_state = callback;
    _callbacks._live_state_userdata = userdata;
    return ONE_ERROR_NONE;
}

Error Client::set_application_instance_status_callback(
    std::function<void(void *, int)> callback, void *userdata) {
    const std::lock_guard<std::mutex> lock(_client);

    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    _callbacks._application_instance_status = callback;
    _callbacks._application_instance_status_userdata = userdata;
    return ONE_ERROR_NONE;
}

Error Client::process_incoming_message(const Message &message) {
    switch (message.code()) {
        case Opcode::live_state:
            if (_callbacks._live_state == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::live_state(message, _callbacks._live_state,
                                          _callbacks._live_state_userdata);
        case Opcode::application_instance_status:
            if (_callbacks._application_instance_status == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::application_instance_status(
                message, _callbacks._application_instance_status,
                _callbacks._application_instance_status_userdata);
        default:
            return ONE_ERROR_NONE;
    }
}

Error Client::process_outgoing_message(const Message &message) {
    Error err = ONE_ERROR_NONE;
    switch (message.code()) {
        case Opcode::soft_stop: {
            params::SoftStopRequest params;
            err = validation::soft_stop(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        case Opcode::allocated: {
            params::AllocatedRequest params;
            err = validation::allocated(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        case Opcode::metadata: {
            params::MetaDataRequest params;
            err = validation::metadata(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        default:
            return ONE_ERROR_NONE;
    }

    if (_connection == nullptr) {
        return ONE_ERROR_VALIDATION_CONNECTION_IS_NULLPTR;
    }
    // Do not accumulate messages if the connection is not active and past
    // handshaking.
    if (_connection->status() != Connection::Status::ready) {
        return ONE_ERROR_SERVER_CONNECTION_NOT_READY;
    }

    err = _connection->add_outgoing(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
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

    auto err = _socket->connect(_server_address.c_str(), _server_port);
    if (is_error(err)) {
        return err;
    }

    _is_connected = true;
    return ONE_ERROR_NONE;
}

}  // namespace one
}  // namespace i3d
