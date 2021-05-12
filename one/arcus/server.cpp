#include <one/arcus/server.h>

#include <one/arcus/allocator.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/messages.h>
#include <one/arcus/internal/mutex.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/opcode.h>
#include <one/arcus/message.h>

#define ONE_ARCUS_SERVER_LOGGING

namespace i3d {
namespace one {

namespace {
size_t listen_retry_delay_seconds = 60;
}

namespace server {
// For testing.
void set_listen_retry_delay(size_t seconds) {
    listen_retry_delay_seconds = seconds;
}
}  // namespace server

bool Server::game_states_changed(Server::GameState &new_state,
                                 Server::GameState &old_state) {
    if (new_state.players != old_state.players) {
        return true;
    }
    if (new_state.max_players != old_state.max_players) {
        return true;
    }
    if (new_state.name != old_state.name) {
        return true;
    }
    if (new_state.map != old_state.map) {
        return true;
    }
    if (new_state.mode != old_state.mode) {
        return true;
    }
    if (new_state.version != old_state.version) {
        return true;
    }
    return false;
}

// See: https://en.cppreference.com/w/cpp/language/value_initialization
// C++11 Value initialization
Server::Server()
    : _listen_port(0)
    , _is_listening(false)
    , _listen_socket(nullptr)
    , _client_socket(nullptr)
    , _client_connection(nullptr)
    , _is_waiting_for_client(false)
    , _game_state()
    , _last_sent_game_state()
    , _game_state_was_set(false)
    , _status(ApplicationInstanceStatus::starting)
    , _should_send_status(false)
    , _callbacks{}
    , _last_listen_attempt_time(steady_clock::duration::zero()) {}

Server::~Server() {
    shutdown();
}

void Server::set_logger(const Logger &logger) {
    _logger = logger;
}

OneError Server::init(unsigned int listen_port) {
    const std::lock_guard<std::mutex> lock(_server);

    _listen_port = listen_port;

    if (_listen_socket != nullptr || _client_socket != nullptr ||
        _client_connection != nullptr) {
        return ONE_ERROR_SERVER_ALREADY_INITIALIZED;
    }

    auto err = init_socket_system();
    if (is_error(err)) {
        return err;
    }

    _listen_socket = allocator::create<Socket>();
    if (_listen_socket == nullptr) {
        return ONE_ERROR_SERVER_SOCKET_ALLOCATION_FAILED;
    }

    err = _listen_socket->init();
    if (is_error(err)) {
        shutdown();
        return err;
    }

    _client_socket = allocator::create<Socket>();
    if (_client_socket == nullptr) {
        shutdown();
        return ONE_ERROR_SERVER_SOCKET_ALLOCATION_FAILED;
    }

    const auto max_incoming = Connection::max_message_default;
    const auto max_outgoing = Connection::max_message_default;
    _client_connection = allocator::create<Connection>(max_incoming, max_outgoing);
    if (_client_connection == nullptr) {
        shutdown();
        return ONE_ERROR_SERVER_CONNECTION_IS_NULLPTR;
    }

    // Attempt to start listening at init time, but if port binding fails then
    // update will try to listen again periodically, so punt the bind error
    // to update calls since init has technically succeeded with this behavior.
    err = listen();
    if (is_error(err) && err != ONE_ERROR_SOCKET_BIND_FAILED) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError Server::shutdown() {
    _logger.Log(LogLevel::Info, "server is shutting down");

    const std::lock_guard<std::mutex> lock(_server);

    if (_client_connection != nullptr) {
        allocator::destroy<Connection>(_client_connection);
        _client_connection = nullptr;
    }

    if (_listen_socket != nullptr) {
        allocator::destroy<Socket>(_listen_socket);
        _listen_socket = nullptr;
    }

    if (_client_socket != nullptr) {
        allocator::destroy<Socket>(_client_socket);
        _client_socket = nullptr;
    }

    shutdown_socket_system();
    ServerCallbacks cb{};
    _callbacks = cb;
    return ONE_ERROR_NONE;
}

String Server::status_to_string(Status status) {
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
    const std::lock_guard<std::mutex> lock(_server);

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

OneError Server::listen() {
    if (_listen_socket == nullptr) {
        return ONE_ERROR_SERVER_SOCKET_IS_NULLPTR;
    }

    if (!_listen_socket->is_initialized()) {
        return ONE_ERROR_SERVER_SOCKET_NOT_INITIALIZED;
    }

    if (_is_listening) {
        return ONE_ERROR_SERVER_ALREADY_LISTENING;
    }

    const auto now = steady_clock::now();
    const size_t delta = duration_cast<seconds>(now - _last_listen_attempt_time).count();
    if (delta <= listen_retry_delay_seconds) {
        return ONE_ERROR_SERVER_RETRYING_LISTEN;
    }
    _last_listen_attempt_time = now;

    auto err = _listen_socket->bind(_listen_port);
    if (is_error(err)) {
#ifdef ONE_ARCUS_SERVER_LOGGING
        _listen_socket->set_last_error_text();
        OStringStream stream;
        stream << "failed to bind socket with error text: "
               << _listen_socket->last_error_text();
        _logger.Log(LogLevel::Error, stream.str());
#endif

        return err;
    }

    err = _listen_socket->listen(Socket::default_queue_length);
    if (is_error(err)) {
        return err;
    }

    _is_listening = true;
    _is_waiting_for_client = true;

    return ONE_ERROR_NONE;
}

bool Server::is_initialized() const {
    return (_listen_socket != nullptr);
}

OneError Server::update_listen_socket() {
    assert(_listen_socket != nullptr);

    if (!_is_listening) {
        auto err = listen();
        if (is_error(err)) {
            return err;
        }
    }

    bool is_ready;
    auto err = _listen_socket->ready_for_read(0.f, is_ready);
    if (is_error(err)) {
        return err;
    }
    if (!is_ready) {
        return ONE_ERROR_NONE;
    }

    String client_ip;
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

    // If a client is already connected, then override the existing connection.
    if (_client_socket->is_initialized()) {
        close_client_connection();
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

OneError Server::process_incoming_message(const Message &message) {
    // Unlock and relock the server mutex when processing incoming messages to
    // allow the callback to be re-entrant on server functions (e.g. to send
    // an outgoing message in response to an incoming message).
    const ReverseLockGuard<std::mutex> reverse_lock(_server);

#ifdef ONE_ARCUS_SERVER_LOGGING
    OStringStream stream;
    stream << "incoming opcode: " << static_cast<int>(message.code())
           << ", payload: " << message.payload().to_json();
    _logger.Log(LogLevel::Info, stream.str());
#endif

    switch (message.code()) {
        case Opcode::soft_stop:
            if (_callbacks._soft_stop == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::soft_stop(message, _callbacks._soft_stop,
                                         _callbacks._soft_stop_userdata);
        case Opcode::allocated:
            if (_callbacks._allocated == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::allocated(message, _callbacks._allocated,
                                         _callbacks._allocated_userdata);
        case Opcode::metadata:
            if (_callbacks._metadata == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::metadata(message, _callbacks._metadata,
                                        _callbacks._metadata_userdata);
        case Opcode::host_information:
            if (_callbacks._host_information == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::host_information(message, _callbacks._host_information,
                                                _callbacks._host_information_data);
        case Opcode::application_instance_information:
            if (_callbacks._application_instance_information == nullptr) {
                return ONE_ERROR_NONE;
            }

            return invocation::application_instance_information(
                message, _callbacks._application_instance_information,
                _callbacks._application_instance_information_data);
        default:
            return ONE_ERROR_NONE;
    }
}

OneError Server::process_outgoing_message(const Message &message) {
#ifdef ONE_ARCUS_SERVER_LOGGING
    OStringStream stream;
    stream << "outgoing opcode: " << static_cast<int>(message.code())
           << ", payload: " << message.payload().to_json();
    _logger.Log(LogLevel::Info, stream.str());
#endif

    OneError err = ONE_ERROR_NONE;
    switch (message.code()) {
        case Opcode::live_state: {
            params::LiveStateResponse params;
            err = validation::live_state(message, params);
            if (is_error(err)) {
                return err;
            }

            break;
        }
        case Opcode::application_instance_status: {
            params::ApplicationInstanceSetStatusRequest params;
            err = validation::application_instance_status(message, params);
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
    // Do not accumulate messages if the connection is not active and past
    // handshaking.
    if (_client_connection->status() != Connection::Status::ready) {
        return ONE_ERROR_SERVER_CONNECTION_NOT_READY;
    }

    err = _client_connection->add_outgoing(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

void Server::close_client_connection() {
    _client_connection->shutdown();
    _client_socket->close();
    _is_waiting_for_client = true;

#ifdef ONE_ARCUS_SERVER_LOGGING
    String ip;
    unsigned int port;
    _client_socket->address(ip, port);
    OStringStream stream;
    stream << "closing client ip: " << ip << ", port: " << std::to_string(port);
    _logger.Log(LogLevel::Info, stream.str());
#endif
}

OneError Server::update_client_connection() {
    // If any errors are encountered while updating the connection, then close
    // the connection and socket. The client is expected to reconnect.
    auto fail = [this](const OneError passthrough_err) -> OneError {
        close_client_connection();
        return passthrough_err;
    };

    // Updating the connection will send pending outgoing messages and gather
    // incoming messages for reading.
    auto err = _client_connection->update();
    if (is_error(err)) {
        return fail(err);
    }

    // Read pending incoming messages.
    while (true) {
        unsigned int count = 0;
        err = _client_connection->incoming_count(count);
        if (is_error(err)) return fail(err);

        if (count == 0) break;

#ifdef ONE_ARCUS_SERVER_LOGGING
        OStringStream stream;
        stream << "server processing incoming messages: " << count;
        _logger.Log(LogLevel::Info, stream.str());
#endif

        err = _client_connection->remove_incoming(
            [this](const Message &message) { return process_incoming_message(message); });
        if (is_error(err)) return fail(err);
    }

    return ONE_ERROR_NONE;
}

OneError Server::update() {
    const std::lock_guard<std::mutex> lock(_server);

    if (!is_initialized()) {
        return ONE_ERROR_SERVER_SOCKET_NOT_INITIALIZED;
    }

    assert(_client_socket != nullptr);
    assert(_client_connection != nullptr);

    auto err = update_listen_socket();
    if (is_error(err)) {
        return err;
    }

    // Done if no client is connected.
    if (!_client_socket->is_initialized()) {
        return ONE_ERROR_NONE;
    }

    const bool was_ready = (_client_connection->status() == Connection::Status::ready);

    if (was_ready && _game_state_was_set) {
        if (game_states_changed(_game_state, _last_sent_game_state)) {
            if (_client_connection->status() == Connection::Status::ready) {
                err = send_live_state();
                if (is_error(err)) {
                    close_client_connection();
                    return err;
                }
                _game_state_was_set = false;
                _last_sent_game_state = _game_state;
            }
        } else {
            _game_state_was_set = false;
        }
    }
    if (was_ready && _should_send_status) {
        err = send_application_instance_status();
        if (is_error(err)) {
            close_client_connection();
            return err;
        }
        _should_send_status = false;
    }

    err = update_client_connection();
    if (is_error(err)) {
        return err;
    }

    const bool is_ready = (_client_connection->status() == Connection::Status::ready);
    if (is_ready && !was_ready) {
        // Schedule a send when connection is established to ensure newly
        // connected client has the correct state.
        _game_state_was_set = true;
        _should_send_status = true;
    }

    return ONE_ERROR_NONE;
}

OneError Server::set_live_state(int players, int max_players, const char *name,
                                const char *map, const char *mode, const char *version,
                                Object *additional_data) {
    const std::lock_guard<std::mutex> lock(_server);

    _game_state.players = players;
    _game_state.max_players = max_players;
    _game_state.name = name;
    _game_state.map = map;
    _game_state.mode = mode;
    _game_state.version = version;
    _game_state_was_set = true;

    return ONE_ERROR_NONE;
}

OneError Server::set_application_instance_status(ApplicationInstanceStatus status) {
    const std::lock_guard<std::mutex> lock(_server);

    if (status == _status) return ONE_ERROR_NONE;

    _status = status;
    _should_send_status = true;

    return ONE_ERROR_NONE;
}

OneError Server::set_soft_stop_callback(std::function<void(void *, int)> callback,
                                        void *data) {
    const std::lock_guard<std::mutex> lock(_server);

    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._soft_stop = callback;
    _callbacks._soft_stop_userdata = data;
    return ONE_ERROR_NONE;
}

OneError Server::send_live_state() {
    Message message;
    auto err = messages::prepare_live_state(
        _game_state.players, _game_state.max_players, _game_state.name.c_str(),
        _game_state.map.c_str(), _game_state.mode.c_str(), _game_state.version.c_str(),
        message);

    if (is_error(err)) {
        return err;
    }

    err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError Server::send_application_instance_status() {
    Message message;
    OneError err = messages::prepare_application_instance_status((int)_status, message);
    if (is_error(err)) {
        return err;
    }

    err = process_outgoing_message(message);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError Server::set_allocated_callback(std::function<void(void *, Array *)> callback,
                                        void *data) {
    const std::lock_guard<std::mutex> lock(_server);

    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._allocated = callback;
    _callbacks._allocated_userdata = data;
    return ONE_ERROR_NONE;
}

OneError Server::set_metadata_callback(std::function<void(void *, Array *)> callback,
                                       void *data) {
    const std::lock_guard<std::mutex> lock(_server);

    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._metadata = callback;
    _callbacks._metadata_userdata = data;
    return ONE_ERROR_NONE;
}

OneError Server::set_host_information_callback(
    std::function<void(void *, Object *)> callback, void *data) {
    const std::lock_guard<std::mutex> lock(_server);

    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._host_information = callback;
    _callbacks._host_information_data = data;
    return ONE_ERROR_NONE;
}

OneError Server::set_application_instance_information_callback(
    std::function<void(void *, Object *)> callback, void *data) {
    const std::lock_guard<std::mutex> lock(_server);

    if (callback == nullptr) {
        return ONE_ERROR_SERVER_CALLBACK_IS_NULLPTR;
    }

    _callbacks._application_instance_information = callback;
    _callbacks._application_instance_information_data = data;
    return ONE_ERROR_NONE;
}

}  // namespace one
}  // namespace i3d
