// A connection manages an arcus socket and incoming/outgoing messages.

namespace one {

class Socket;
class Message;
template<typename T>
class RingBuffer;

// Connection is a persistent arcus TCP connection. It can send and receive
// arbitrary arcus messages.
class Connection final {
public:
    Connection(Socket& socket);
    
    void update(int* error);
    int status();

    void push_outgoing(Message* message, int* error);
    int incoming_count();
    Message* pop_incoming(int* error);
private:
    Connection();
    
    //RingBuffer<Message*> _incomingMessages;
    //RingBuffer<Message*> _outgoingMessages;
    // Outgoing messages.
};

} // namespace one