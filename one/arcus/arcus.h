#pragma once

#include <functional>
#include <string>

namespace one {

class Array;
class Connection;
class Message;
class Socket;

// An Arcus Server is designed for use by a Game. It allows an Arcus One Agent
// to connect and communicate with the game.
class Server final {
public:
    Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;
    ~Server();

    int init();

    int shutdown();

    int status() const;

    int listen(unsigned int port);

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

    // set the callback for when a live_state_request message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    int set_live_state_request_callback(std::function<void(void *)> callback, void *data);

    // all other externally-facing opcode callbacks...

    //------------------------------------------------------------------------------
    // Outgoing.

    // Todo: update functions to match complete list from One API v2.

    // send live_state opcode message.
    // Message Mandatory Content:
    // {
    //   players : 0,
    //   max players : 0,
    //   server name : "",
    //   server map : "",
    //   server mode : "",
    //   server version : "",
    // }
    int send_live_state(const Message &message);

private:
    int process_incoming_message(const Message &message);
    int process_outgoing_message(const Message &message);

    Socket *_listen_socket;
    Socket *_client_socket;
    Connection *_client_connection;

    std::function<void(void *, int)> _soft_stop_callback;
    void *_soft_stop_callback_data;
    std::function<void(void *)> _live_state_request_callback;
    void *_live_state_request_callback_data;
};

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

    int send_soft_stop();
    int request_server_info();

    //------------------------------------------------------------------------------
    // Callbacks to be notified of all possible incoming Arcus messages.

    // Todo: update functions to match complete list from One API v2.

    int set_live_state_callback(
        std::function<void(int, int, const std::string &, const std::string &, const std::string &,
                           const std::string &)>
            callback);

private:
    Socket *_socket;
    Connection *_connection;

    std::function<void(int, int, const std::string &, const std::string &, const std::string &,
                       const std::string &)>
        _live_state_callback;
};

}  // namespace one
