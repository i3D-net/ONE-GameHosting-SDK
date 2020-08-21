#pragma once

#include <functional>
#include <string>

namespace one {

class Array;
class Connection;
class Message;
class Socket;

namespace callback {

struct ClientCallbacks {
    std::function<void(void *)> _error_response;
    void *_error_response_data;
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        _live_state_response;
    void *_live_state_response_data;
    std::function<void(void *)> _host_information_request;
    void *_host_information_request_data;
};

}  // namespace callback

// The Arcus Client is used by an Arcus One Agent to connect to an Arcus Server.
class Client final {
public:
    Client();
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
    ~Client();

    int init();
    int shutdown();
    int connect(const char *address, unsigned int port);
    int update();

    enum class Status { handshake, ready, error };
    Status status();

    //-------------------
    // Outgoing Messages.

    // Todo: update functions to match complete list from One API v2.

    int send_soft_stop_request(int timeout);
    int send_allocated_request(Array *data);
    int send_meta_data_request(Array *data);
    int send_live_state_request();

    //------------------------------------------------------------------------------
    // Callbacks to be notified of all possible incoming Arcus messages.

    // Todo: update functions to match complete list from One API v2.

    // set the callback for when a error_response message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    int set_error_callback(std::function<void(void *)> callback, void *data);

    // set the callback for when a live_state_response message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    int set_live_state_callback(
        std::function<void(void *, int, int, const std::string &, const std::string &,
                           const std::string &, const std::string &)>
            callback,
        void *data);

    // set the callback for when a host_information_request message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    int set_host_information_request_callback(std::function<void(void *)> callback, void *data);

private:
    int process_incoming_message(const Message &message);
    int process_outgoing_message(const Message &message);

    Socket *_socket;
    Connection *_connection;

    callback::ClientCallbacks _callbacks;
};

}  // namespace one
