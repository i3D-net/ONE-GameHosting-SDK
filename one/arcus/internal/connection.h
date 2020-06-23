#pragma once

namespace one {

class Socket;
class Message;
template<typename T>
class RingBuffer;

// Connection is a persistent arcus TCP connection. It can send and receive
// arbitrary arcus messages.
// It:
// - reads from the socket to create incoming message pools
// - provides interface to pop incoming messages
// - provides interface to push outgoing messages for sending
// - sends outgoing messages to the socket
// - uses a codec to translate to/from byte data and Message types
class Connection final {
public:
    Connection(Socket& socket, size_t max_incoming, size_t max_outgoing);

    void update(int* error);
    int status() const;

    void push_outgoing(Message* message, int* error);

    int incoming_count() const;

    Message* pop_incoming(int* error);
private:
    Connection();
    
    //RingBuffer<Message*> _incomingMessages;
    //RingBuffer<Message*> _outgoingMessages;
    // Outgoing messages.
};

// Listener listens on a socket for an incoming arcus client connection.
class Listener final {
    Listener();

    void listen(const char ip, unsigned int port);
    void shutdown();
    int status();

    // Returns the active client connection, if status is active.
    Connection& client();
};

} // namespace one
