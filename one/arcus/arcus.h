#pragma once

#include <functional>

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

    void set_soft_stop_callback(std::function<void(int)> callback);

    void set_live_state_request_callback(std::function<void()> callback);

    // all other externally-facing opcode callbacks...

    //------------------------------------------------------------------------------
    // Outgoing.

    // Todo: update functions to match complete list from One API v2.

    // notify one of state change
    // send metadata (key value pairs)

    // server info send(
    //     current players,
    //     max players,
    //     server name,
    //     map,
    //     mode,
    //     version)
    int send_server_info(const Message &message);

private:
    int process_incoming_message(const Message &message);
    int process_outgoing_message(const Message &message);

    Socket *_listen_socket;
    Socket *_client_socket;
    Connection *_client_connection;

    std::function<void(int)> _soft_stop_callback;
    std::function<void()> _live_state_request_callback;
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
    int connect(const char *ip, unsigned int port);
    int update();

    //-------------------
    // Outgoing Messages.

    // todo.

    int send_soft_stop();
    int request_server_info();
    int send(Message *message);

    // ...

    //-------------------
    // Incoming Messages.

    // todo.

private:
    Socket *_socket;
    Connection *_connection;
};

}  // namespace one
