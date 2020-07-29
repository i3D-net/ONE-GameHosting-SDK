#pragma once

#include <functional>


namespace one {

class Array;
class Connection;
class Message;
class Socket;

// An Arcus Server is designed for use by a Game. It allows an Arcus One Agent
// to connect and communicate with the game.
class Server final
{
public:
    Server(/* args */);
    ~Server();

    int listen(unsigned int port);

    // Process pending received and outgoing messages. Any incoming messages are
    // validated according to the Arcus API version standard, and callbacks, if
    // set, are called. Messages without callbacks set are dropped and ignored.
    int update();

    int status() const;

    //------------------------------------------------------------------------------
    // Callbacks to be notified of all possible incoming Arcus messages.

    // Todo: update functions to match complete list from One API v2.

    void set_soft_stop_callback(std::function<void(int)>);

    void set_allocated_callback(std::function<void()>);

    void set_server_info_request_callback(std::function<void()>);

    void set_server_info_callback(std::function<void(int)>);

    void set_custom_command_callback(std::function<void(int)>);

    // TBD: metadata format? why "data" key array?
    void set_metadata_callback(std::function<void(const Array&)>);

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
    void send_server_info(Message&);

private:
    Socket *_listen_socket;
    Socket *_client_socket;
    Connection *_client_connection;
};

// The Arcus Client is used by an Arcus One Agent to connect to an Arcus Server.
class Client final
{
public:
    Client(/* args */);
    ~Client();

    int connect(const char * ip, unsigned int port);

    int update();

    //-------------------
    // Outgoing Messages.

    // todo.

    // send_soft_stop(...)

    // ...

    //-------------------
    // Incoming Messages.

    // todo.

private:
    Socket *_socket;
    Connection *_connection;
};

} // namespace one
