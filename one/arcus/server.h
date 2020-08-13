#pragma once

#include <functional>
#include <string>

namespace one {

class Array;
class Connection;
class Message;
class Socket;

namespace callback {

struct ServerCallbacks {
    std::function<void(void *, int)> _soft_stop_request;
    void *_soft_stop_request_data;
    std::function<void(void *, Array *)> _allocated_request;
    void *_allocated_request_data;
    std::function<void(void *, Array *)> _meta_data_request;
    void *_meta_data_request_data;
    std::function<void(void *)> _live_state_request;
    void *_live_state_request_data;
};

}  // namespace callback

// An Arcus Server is designed for use by a Game. It allows an Arcus One Agent
// to connect and communicate with the game.
class Server final {
public:
    Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;
    ~Server();

    int init(size_t max_message_in, size_t max_message_out);

    int shutdown();

    enum class Status { listening, handshake, ready, error };
    Status status() const;

    int listen(unsigned int port, int queueLength);

    // Process pending received and outgoing messages. Any incoming messages are
    // validated according to the Arcus API version standard, and callbacks, if
    // set, are called. Messages without callbacks set are dropped and ignored.
    int update();

    //------------------------------------------------------------------------------
    // Callbacks to be notified of all possible incoming Arcus messages.

    // Todo: update functions to match complete list from One API v2.

    // set the callback for when a soft_stop message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    int set_soft_stop_callback(std::function<void(void *, int)> callback, void *data);

    // set the callback for when a allocated message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    int set_allocated_callback(std::function<void(void *, Array *)> callback, void *data);

    // set the callback for when a meta_data message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    int set_meta_data_callback(std::function<void(void *, Array *)> callback, void *data);

    // set the callback for when a live_state_request message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    int set_live_state_request_callback(std::function<void(void *)> callback, void *data);

    // all other externally-facing opcode callbacks...

    //------------------------------------------------------------------------------
    // Outgoing.

    // Todo: update functions to match complete list from One API v2.

    // send error opcode message.
    // Message Empty Content:
    // {}
    int send_error_response(const Message &message);

    // send live_state_response opcode message.
    // Message Mandatory Content:
    // {
    //   players : 0,
    //   max players : 0,
    //   server name : "",
    //   server map : "",
    //   server mode : "",
    //   server version : "",
    // }
    int send_live_state_response(const Message &message);

    // send host_information_reqeust.
    // Message Empty Content:
    // {}
    int send_host_information_request(const Message &message);

private:
    int process_incoming_message(const Message &message);
    int process_outgoing_message(const Message &message);

    Socket *_listen_socket;
    Socket *_client_socket;
    Connection *_client_connection;

    callback::ServerCallbacks _callbacks;
};

}  // namespace one