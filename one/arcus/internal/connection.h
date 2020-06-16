// A connection manages an arcus socket and incoming/outgoing messages.

namespace one {

class Message;

class MessageQueue final {
public:
    // Todo
    // - in constructor, preallocate the message buffer to some high amount
    // that should never be hit in regular use cases
    // - 

    void PushBack(Message*, int& error);
    int Count();
    Message* PopFront(int& error);
private:
    // Messages.
    // _ring_buffer<Message>;
};

class Connection final {
public:
    // Send(message)
    // int IncomingCount()
    // PopIncoming()
private:
    // Incoming messages.
    // Outgoing messages.
};

} // namespace one