#pragma once

#include <functional>

namespace one {

class Message;
class Array;
///
/// Arcus is the main interface for an Arcus API Connection.
/// It manages a TCP listen connection, allowing a single Agent client to connect
/// and send/receive messages.
///
class Arcus
{
public:
    Arcus(/* args */);
    ~Arcus();

    // Process pending received and outgoing messages. Any incoming messages are
    // validated according to the Arcus API version standard, and callbacks, if
    // set, are called. Messages without callbacks set are dropped and ignored.
    void update(int* error);

    int status() const;

    //------------------------------------------------------------------------------
    // Callbacks to be notified of all possible incoming Arcus messages.

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

    // WIP.

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
};

} // namespace one
