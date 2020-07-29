#pragma once

#include "platform.h"

namespace one {

class Socket;
class Message;
template<typename T>
class RingBuffer;

// Connection manages Arcus protocol communication between two TCP sockets.
class Connection final {
public:
    // Connection must be given an active socket. Socket errors encountered
    // during processing will be returned as errors, and it is the caller's
    // responsibilty to either destroy the Connection, or restore the Socket's
    // state for communication.
    Connection(Socket& socket, size_t max_messages_in, size_t max_messages_out);
    ~Connection();

    // Start handshake process. Should be called from one side of the connection
    // only. No messages can be sent from either side until handshaking is
    // complete.
    void start_handshake();

    // Update process incoming and outgoing messges. It attempts to read
    // all incoming messages that are available. It attempts to send all
    // queued outgoing messages.
    int update();

    void push_outgoing(Message* message, int* error);

    int incoming_count() const;

    int pop_incoming(Message **message);

private:
    Connection() = delete;

    int process_handshake();

    int process_messages();

    Socket &_socket;
    enum HandshakeStatus {
        init = 0,
        hello_sent,
        complete
    };
    HandshakeStatus _handshake_status;

    //RingBuffer<Message*> _incomingMessages;
    //RingBuffer<Message*> _outgoingMessages;
};

} // namespace one
