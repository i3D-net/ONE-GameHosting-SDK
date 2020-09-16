#include <one/arcus/server.h>

#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/message.h>
#include <one/arcus/opcode.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/message.h>

namespace i3d {
namespace one {

Server::Server()
    : _listen_socket(nullptr)
    , _client_socket(nullptr)
    , _client_connection(nullptr)
    , _is_waiting_for_client(false)
    , _callbacks({0}) {}

Server::~Server() {
    shutdown();
}

Error Server::init() {
    if (_listen_socket != nullptr || _client_socket != nullptr ||
        _client_connection != nullptr) {
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

    _client_connection =
        new Connection(Connection::max_message_default, Connection::max_message_default);
    if (_client_connection == nullptr) {
        shutdown();
        return ONE_ERROR_SERVER_CONNECTION_IS_NULLPTR;
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

std::string Server::status_to_string(Status status) {
    switch (status) {
        case Status::uninitialized:
            return "uninitialized";
        case Status::initialized:
            return "initialized";
        case Status::waiting_for_client:
            return "waiting_for_client";
        case Status::handshake:
            return "handshake";
        case Status::ready:
            return "ready";
        case Status::error:
            return "error";
    }
    return "unknown";
}

Server::Status Server::status() const {
    if (!is_initialized()) return Status::uninitialized;

    if (_is_waiting_for_client) return Status::waiting_for_client;

    if (_listen_socket->is_initialized() && !_is_waiting_for_client &&
        !_client_socket->is_initialized()) {
        return Status::initialized;
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

Error Server::listen(unsigned int port) {
    if (_listen_socket == nullptr) {
        return ONE_ERROR_SERVER_SOCKET_IS_NULLPTR;
    }

    if (!_listen_socket->is_initialized()) {
        return ONE_ERROR_SERVER_SOCKET_NOT_INITIALIZED;
    }

    auto err = _listen_socket->bind(port);
    if (is_error(err)) {
        return err;
    }

    err = _listen_socket->listen(Socket::default_queue_length);
    if (is_error(err)) {
        return err;
    }

    _is_waiting_for_client = true;

    return ONE_ERROR_NONE;
}

Error Server::update_listen_socket() {
    assert(_listen_socket != nullptr);

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
    Socket incoming_client;
    err = _listen_socket->accept(incoming_client, client_ip, client_port);
    if (is_error(err)) {
        return ONE_ERROR_NONE;
    }

    // If no client was accepted, exit.
    if (!incoming_client.is_initialized()) {
        return ONE_ERROR_NONE;
    }

    // If a client is already connected, then close the incoming connection.
    if (_client_socket->is_initialized()) {
        incoming_client.close();
        return ONE_ERROR_NONE;
    }

    // Client accepted, add it.

    _is_waiting_for_client = false;

    *_client_socket = incoming_client;
    _client_connection->init(*_client_socket);

    // The Arcus Server is responsible for initiating the handshake against agents.
    // The agent waits for an initial hello packet from the Server.
    _client_connection->initiate_handshake();

    return ONE_ERROR_NONE;
}

bool Server::is_initialized() const {
    return (_listen_socket != nullptr);
}

Error Server::update() {
    if (!is_initialized()) {
        return ONE_ERROR_SERVER_SOCKET_NOT_INITIALIZED;
    }
    assert(_client_socket != nullptr);
    assert(_client_connection != nullptr);

    auto err = update_listen_socket();
    if (is_error(err)) {
        // Todo: put listening in update loop and close/recover here...
        return err;
    }

    // Done if no client is connected.
    if (!_client_socket->is_initialized()) {
        return ONE_ERROR_NONE;
    }

    // If any errors are encountered while updating the connection, then close
    // the connection and socket. The client is expected to reconnect.
    auto close_client = [this](const Error passthrough_err) -> Error {
        _client_connection->shutdown();
        _client_socket->close();
        _is_waiting_for_client = true;
        return passthrough_err;
    };

    // Updating the connection will send pending outgoing messages and gather incoming
    // messages for reading.
    err = _client_connection->update();
    if (is_error(err)) {
        return close_client(err);
    }

    // Read pending incoming messages.
    while (true) {
        unsigned int count = 0;
        err = _client_connection->incoming_count(count);
        if (is_error(err)) return close_client(err);
        if (count == 0) break;

        err = _client_connection->remove_incoming([this](const Message &message) {
            auto err = process_incoming_message(message);
            if (is_error(err)) return err;

            return ONE_ERROR_NONE;
        });
        if (is_error(err)) {
            return close_client(err);
        }
    }

    return ONE_ERROR_NONE;
}

Error Server::set_soft_stop_callback(std::function<void(void *, int)> callback,
                                     void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._soft_stop_request = callback;
    _callbacks._soft_stop_request_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_allocated_callback(std::function<void(void *, Array *)> callback,
                                     void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._allocated_request = callback;
    _callbacks._allocated_request_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_meta_data_callback(std::function<void(void *, Array *)> callback,
                                     void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._meta_data_request = callback;
    _callbacks._meta_data_request_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_live_state_request_callback(std::function<void(void *)> callback,
                                              void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._live_state_request = callback;
    _callbacks._live_state_request_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_host_information_response_callback(
    std::function<void(void *, Object *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._host_information_response = callback;
    _callbacks._host_information_response_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_application_instance_information_response_callback(
    std::function<void(void *, Object *)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._application_instance_information_response = callback;
    _callbacks._application_instance_information_response_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_application_instance_get_status_response_callback(
    std::function<void(void *, int)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._application_instance_get_status_response = callback;
    _callbacks._application_instance_get_status_response_data = data;
    return ONE_ERROR_NONE;
}

Error Server::set_application_instance_set_status_response_callback(
    std::function<void(void *, int)> callback, void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._application_instance_set_status_response = callback;
    _callbacks._application_instance_set_status_response_data = data;
    return ONE_ERROR_NONE;
}

Error Server::send_live_state_response(const Message &message) {
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Server::send_player_joined_event_response(const Message &message) {
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Server::send_player_left_response(const Message &message) {
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

Error Server::send_application_instance_information_request(const Message &message) {
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Server::send_application_instance_get_status_request(const Message &message) {
    auto err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Server::send_application_instance_set_status_request(const Message &message) {
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
        case Opcode::host_information_response:
            if (_callbacks._host_information_response == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::host_information_response(
                message, _callbacks._host_information_response,
                _callbacks._host_information_response_data);
        case Opcode::application_instance_information_response:
            if (_callbacks._application_instance_information_response == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::application_instance_information_response(
                message, _callbacks._application_instance_information_response,
                _callbacks._application_instance_information_response_data);
        case Opcode::application_instance_get_status_response:
            if (_callbacks._application_instance_get_status_response == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::application_instance_get_status_response(
                message, _callbacks._application_instance_get_status_response,
                _callbacks._application_instance_get_status_response_data);
        case Opcode::application_instance_set_status_response:
            if (_callbacks._application_instance_get_status_response == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::application_instance_set_status_response(
                message, _callbacks._application_instance_set_status_response,
                _callbacks._application_instance_set_status_response_data);
        default:
            return ONE_ERROR_NONE;
    }
}

Error Server::process_outgoing_message(const Message &message) {
    Error err = ONE_ERROR_NONE;
    switch (message.code()) {
        case Opcode::live_state_response: {
            params::LiveStateResponse params;
            err = validation::live_state_response(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        case Opcode::player_joined_event_response: {
            params::PlayerJoinedEventResponse params;
            err = validation::player_joined_event_response(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        case Opcode::player_left_response: {
            params::PlayerLeftResponse params;
            err = validation::player_left_response(message, params);
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
        case Opcode::application_instance_information_request: {
            params::ApplicationInstanceInformationRequest params;
            err = validation::application_instance_information_request(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        case Opcode::application_instance_get_status_request: {
            params::ApplicationInstanceGetStatusRequest params;
            err = validation::application_instance_get_status_request(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        case Opcode::application_instance_set_status_request: {
            params::ApplicationInstanceSetStatusRequest params;
            err = validation::application_instance_set_status_request(message, params);
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

    err = _client_connection->add_outgoing([&](Message &m) {
        m = message;
        return ONE_ERROR_NONE;
    });
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

}  // namespace one
}  // namespace i3d
