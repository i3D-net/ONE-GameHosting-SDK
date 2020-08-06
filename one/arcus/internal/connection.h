#pragma once

#include <one/arcus/internal/platform.h>

namespace one {

class Socket;
class Message;
template <typename T>
class RingBuffer;

// Connection manages Arcus protocol communication between two TCP sockets.
class Connection final {
public:
    // Connection must be given an active socket. Socket errors encountered
    // during processing will be returned as errors, and it is the caller's
    // responsibilty to either destroy the Connection, or restore the Socket's
    // state for communication.
    Connection(Socket& socket, size_t max_messages_in, size_t max_messages_out);
    ~Connection() = default;

    // Marks this side of the connection as responsible for initiating the
    // handshaking process. Must be called from one side of the connection
    // only. Attempting to send a Message or any other data to other side of
    // the Connection before handshaking is complete results in an error.
    void initiate_handshake();

    // Update process incoming and outgoing messges. It attempts to read
    // all incoming messages that are available. It attempts to send all
    // queued outgoing messages.
    int update();

    // Reset the status to match construction time and clear any accumulated
    // buffers.
    void reset();

    enum class Status {
        handshake_not_started,
        handshake_hello_scheduled,
        handshake_hello_sent,
        ready,
        error
    };
    Status status() const;

    int push_outgoing(const Message& message);

    int incoming_count() const;

    int pop_incoming(Message** message);

private:
    Connection() = delete;

    int process_handshake(bool is_socket_ready);
    int process_messages();
    int send(const void* data, size_t length);
    int receive(void* data, size_t length);

    Status _status;
    Socket& _socket;

    // RingBuffer<Message*> _incomingMessages;
    // RingBuffer<Message*> _outgoingMessages;
};

}  // namespace one
