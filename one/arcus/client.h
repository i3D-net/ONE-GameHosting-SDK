#pragma once

#include <chrono>
#include <functional>
#include <mutex>

#include <one/arcus/error.h>
#include <one/arcus/types.h>

using namespace std::chrono;

namespace i3d {
namespace one {

class Array;
class Connection;
class Message;
class Object;
class Socket;

struct ClientCallbacks {
    std::function<void(void *, int, int, const String &, const String &, const String &,
                       const String &)>
        _live_state;
    void *_live_state_userdata;

    std::function<void(void *, int)> _application_instance_status;
    void *_application_instance_status_userdata;
};

// The Arcus Client is used by an Arcus One Agent to connect to an Arcus Server.
class Client final {
public:
    Client();
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
    ~Client();

    Error init(const char *address, unsigned int port);
    void shutdown();
    Error update();

    enum class Status { uninitialized, connecting, handshake, ready, error };
    static String status_to_string(Status status);

    Status status() const;

    //-------------------
    // Outgoing Messages.

    Error send_soft_stop(int timeout);
    Error send_allocated(Array &data);
    Error send_metadata(Array &data);
    Error send_host_information(Object &data);
    Error send_application_instance_information(Object &data);

    //------------------------------------------------------------------------------
    // Callbacks to be notified of all possible incoming Arcus messages.

    // set the callback for when a live_state message in received.
    // The `void *data` is the user provided and will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    Error set_live_state_callback(
        std::function<void(void *, int, int, const String &, const String &,
                           const String &, const String &)>
            callback,
        void *data);

    // set the callback for when an application_instance_status message in
    // received. The `void *data` is the user provided and will be passed as the first
    // argument of the callback when invoked. The `data` can be nullptr, the callback is
    // responsible to use the data properly.
    Error set_application_instance_status_callback(
        std::function<void(void *, int)> callback, void *data);

private:
    Error process_incoming_message(const Message &message);
    // The server must have an active and ready listen connection in order to
    // send outgoing messages. If not, either ONE_ERROR_SERVER_CONNECTION_IS_NULLPTR or
    // ONE_ERROR_SERVER_CONNECTION_NOT_READY is returned and the message is
    // not sent.
    Error process_outgoing_message(const Message &message);

    bool is_initialized() const {
        return _socket != nullptr;
    }

    Error connect();

    mutable std::mutex _client;

    String _server_address;
    unsigned int _server_port;

    Socket *_socket;
    Connection *_connection;
    bool _is_connected;
    ClientCallbacks _callbacks;
    steady_clock::time_point _last_connection_attempt_time;
};

}  // namespace one
}  // namespace i3d
