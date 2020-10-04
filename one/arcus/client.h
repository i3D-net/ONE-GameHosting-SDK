#pragma once

#include <one/arcus/error.h>

#include <chrono>
#include <functional>
#include <mutex>
#include <string>

using namespace std::chrono;

namespace i3d {
namespace one {

class Array;
class Connection;
class Message;
class Socket;

namespace callback {

struct ClientCallbacks {
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        _live_state_response;
    void *_live_state_response_data;
    std::function<void(void *)> _host_information_request;
    void *_host_information_request_data;
    std::function<void(void *)> _application_instance_information_request;
    void *_application_instance_information_request_data;
    std::function<void(void *)> _application_instance_get_status_request;
    void *_application_instance_get_status_request_data;
    std::function<void(void *, int)> _application_instance_set_status_request;
    void *_application_instance_set_status_request_data;
};

}  // namespace callback

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
    static std::string status_to_string(Status status);

    Status status() const;

    //-------------------
    // Outgoing Messages.

    // Todo: update functions to match complete list from One API v2.

    Error send_soft_stop(int timeout);
    Error send_allocated(Array *data);
    Error send_metadata(Array *data);

    //------------------------------------------------------------------------------
    // Callbacks to be notified of all possible incoming Arcus messages.

    // set the callback for when a live_state_response message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    Error set_live_state_response_callback(
        std::function<void(void *, int, int, const std::string &, const std::string &,
                           const std::string &, const std::string &)>
            callback,
        void *data);

    // set the callback for when a host_information_request message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    Error set_host_information_request_callback(std::function<void(void *)> callback,
                                                void *data);

    // set the callback for when an application_instance_information_request message in
    // received. The `void *data` is the user provided & will be passed as the first
    // argument of the callback when invoked. The `data` can be nullptr, the callback is
    // responsible to use the data properly.
    Error set_application_instance_information_request_callback(
        std::function<void(void *)> callback, void *data);

    // set the callback for when an application_instance_get_status_request message in
    // received. The `void *data` is the user provided & will be passed as the first
    // argument of the callback when invoked. The `data` can be nullptr, the callback is
    // responsible to use the data properly.
    Error set_application_instance_get_status_request_callback(
        std::function<void(void *)> callback, void *data);

    // set the callback for when an application_instance_set_status_request message in
    // received. The `void *data` is the user provided & will be passed as the first
    // argument of the callback when invoked. The `data` can be nullptr, the callback is
    // responsible to use the data properly.
    Error set_application_instance_set_status_request_callback(
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

    std::string _server_address;
    unsigned int _server_port;

    Socket *_socket;
    Connection *_connection;
    bool _is_connected;

    steady_clock::time_point _last_connection_attempt_time;

    callback::ClientCallbacks _callbacks;

    mutable std::mutex _client;
};

}  // namespace one
}  // namespace i3d
