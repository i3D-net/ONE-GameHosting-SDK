#include <one/arcus/internal/connection.h>

#include <assert.h>
#include <array>
#include <cstring>

#include <one/arcus/message.h>
#include <one/arcus/internal/codec.h>
#include <one/arcus/opcode.h>
#include <one/arcus/internal/socket.h>

#ifdef WINDOWS
#else
    #include <errno.h>
#endif

namespace one {

// Stream buffers sizes used to pump pending data from/to the connection's
// socket.
constexpr size_t stream_send_buffer_size = 1024 * 128;
constexpr size_t stream_receive_buffer_size = 1024 * 128;
constexpr size_t message_queue_size = 512;

Connection::Connection(Socket &socket, size_t max_messages_in, size_t max_messages_out)
    : _status(Status::handshake_not_started)
    , _socket(socket)
    , _out_stream(stream_send_buffer_size)
    , _in_stream(stream_receive_buffer_size)
    , _incoming_messages(message_queue_size)
    , _outgoing_messages(message_queue_size) {
    assert(socket.is_initialized());
}

Connection::Status Connection::status() const {
    return _status;
}

Error Connection::push_outgoing(Message *message) {
    if (_outgoing_messages.size() == _outgoing_messages.capacity())
        return ONE_ERROR_INSUFFICIENT_SPACE;
    _outgoing_messages.push(message);
    return ONE_ERROR_NONE;
}

Error Connection::incoming_count(unsigned int &count) const {
    count = static_cast<unsigned int>(_incoming_messages.size());
    return ONE_ERROR_NONE;
}

Error Connection::pop_incoming(Message **message) {
    if (_incoming_messages.size() == 0) {
        *message = nullptr;
        return ONE_ERROR_EMPTY;
    }
    *message = _incoming_messages.pop();
    return ONE_ERROR_NONE;
}

void Connection::initiate_handshake() {
    assert(_status == Status::handshake_not_started);
    _status = Status::handshake_hello_scheduled;
}

Error Connection::update() {
    if (_status == Status::error) return ONE_ERROR_CONNECTION_UPDATE_AFTER_ERROR;

    // Return if socket has no activity or has an error.
    bool is_ready = false;
    auto err = _socket.ready_for_send(0.f, is_ready);
    if (is_error(err)) return err;
    if (!is_ready) return ONE_ERROR_NONE;

    if (_status != Status::ready) return process_handshake();

    // The server mostly replies to client request, so send outgoing messages
    // first.
    err = process_outgoing_messages();
    if (is_error(err)) return err;  // Flush incoming also if error on outgoing?
    return process_incoming_messages();
}

void Connection::reset() {
    _out_stream.clear();
    _in_stream.clear();
    _outgoing_messages.clear();
    _incoming_messages.clear();
    _status = Status::handshake_not_started;
}

Error Connection::ensure_nothing_received() {
    char byte;
    size_t received = 0;
    auto err = _socket.receive(&byte, 1, received);
    if (is_error(err)) {
        return err;
    }
    if (received > 0) {
        return ONE_ERROR_CONNECTION_RECEIVE_BEFORE_SEND;
    }
    return ONE_ERROR_NONE;
}

Error Connection::try_send_hello() {
    auto &stream = _out_stream;

    // Buffer outgoing message if not yet done so. Nearly all the time the
    // send will succeed since it is tiny and partial sends
    // are rare edge cases in general.
    if (stream.size() == 0) {
        stream.put(&codec::valid_hello(), codec::hello_size());
    }

    // Get remaining buffer.
    const auto size = stream.size();
    assert(size > 0);
    void *data = nullptr;
    stream.peek(size, &data);
    assert(data != nullptr);

    // Send as much as possible.
    size_t sent = 0;
    auto err = _socket.send(data, size, sent);
    if (is_error(err)) {  // Error.
        return ONE_ERROR_CONNECTION_HELLO_SEND_FAILED;
    }
    if (sent == 0) return ONE_ERROR_TRY_AGAIN;

    // Remove from send stream, check if finished.
    stream.trim(sent);
    if (stream.size() > 0) return ONE_ERROR_TRY_AGAIN;

    return ONE_ERROR_NONE;
}

Error Connection::try_receive_hello() {
    // Read a hello packet from socket.
    codec::Hello hello = {0};
    size_t received = 0;
    auto err = _socket.receive(&hello, codec::hello_size(), received);
    if (is_error(err)) {
        return ONE_ERROR_CONNECTION_HELLO_RECEIVE_FAILED;
    }
    if (received == 0) {             // No error but nothing received.
        return ONE_ERROR_TRY_AGAIN;  // Retry next attempt.
    }
    if (received > codec::hello_size()) {
        return ONE_ERROR_CONNECTION_HELLO_TOO_BIG;
    }

    // Buffer bytes read. This will normally be the entire hello, but
    // there are edge cases that might result in partial reads. Return
    // if the full hello has not been read yet.
    _in_stream.put(&hello, received);
    if (_in_stream.size() < codec::hello_size()) {
        return ONE_ERROR_TRY_AGAIN;
    }

    // Read and validate the full hello from the receive buffer.
    codec::Hello *data;
    _in_stream.get(codec::hello_size(), reinterpret_cast<void **>(&data));
    assert(data != nullptr);

    if (!codec::validate_hello(*data)) {
        return ONE_ERROR_CONNECTION_HELLO_INVALID;
    }
    return ONE_ERROR_NONE;
}

// There are two hello packets. The initial codec::hello sent from the
// handshake initiater, and the response codec::Header message with a
// hello opcode sent in response. This is the response header.
const codec::Header &hello_message() {
    static codec::Header message = {0};
    message.opcode = static_cast<char>(Opcode::hello);
    return message;
}

Error Connection::try_send_hello_message() {
    auto &stream = _out_stream;

    // Buffer outgoing message if not yet done so. Nearly all the time the send
    // will succeed since it is tiny and partial sends are rare edge cases in
    // general.
    if (stream.size() == 0) {
        stream.put(&hello_message(), codec::header_size());
    }

    // Get remaining buffer.
    const auto size = stream.size();
    assert(size > 0);
    void *data = nullptr;
    stream.peek(size, &data);
    assert(data != nullptr);

    // Send.
    size_t sent = 0;
    auto err = _socket.send(data, size, sent);
    if (is_error(err)) {
        return ONE_ERROR_CONNECTION_HELLO_MESSAGE_SEND_FAILED;
    }
    if (sent == 0) return ONE_ERROR_TRY_AGAIN;

    // Remove from send stream, check if finished.
    stream.trim(sent);
    if (stream.size() > 0) return ONE_ERROR_TRY_AGAIN;

    return ONE_ERROR_NONE;
}

Error Connection::try_receive_message_header(codec::Header &header) {
    size_t received = 0;
    auto err = _socket.receive(&header, codec::header_size(), received);
    if (is_error(err)) {
        _status = Status::error;
        return ONE_ERROR_CONNECTION_HELLO_MESSAGE_RECEIVE_FAILED;
    }
    if (received > codec::header_size()) {
        _status = Status::error;
        return ONE_ERROR_CONNECTION_HELLO_MESSAGE_HEADER_TOO_BIG;
    }

    // Buffer bytes read.
    _in_stream.put(&header, received);
    if (_in_stream.size() < codec::header_size()) {
        return ONE_ERROR_TRY_AGAIN;
    }

    // Read the full hello from the receive buffer.
    codec::Header *data = nullptr;
    _in_stream.get(codec::header_size(), reinterpret_cast<void **>(&data));
    assert(data != nullptr);
    memcpy(&header, data, codec::header_size());

    if (!codec::validate_header(*data)) return ONE_ERROR_CONNECTION_INVALID_MESSAGE_HEADER;

    return ONE_ERROR_NONE;
}

Error Connection::try_receive_hello_message() {
    codec::Header header = {0};
    auto err = try_receive_message_header(header);
    if (is_error(err)) {
        return err;
    }

    if (std::memcmp(&header, &hello_message(), codec::header_size()) != 0) {
        _status = Status::error;
        return ONE_ERROR_CONNECTION_HELLO_MESSAGE_REPLY_INVALID;
    }
    return ONE_ERROR_NONE;
}

Error Connection::process_handshake() {
    // Check if handshake timed out.
    // return ONE_ERROR_NONE;

    Error err;
    auto fail = [this](Error err) {
        _status = Status::error;
        return err;
    };

    switch (_status) {
        case Status::handshake_not_started:
            // Check if hello received.
            err = try_receive_hello();
            if (err == ONE_ERROR_TRY_AGAIN) break;
            if (is_error(err)) return fail(err);

            {
                bool is_ready = false;
                err = _socket.ready_for_send(0.f, is_ready);
                if (is_error(err)) return err;
                if (!is_ready) break;
            }
            // Fallthrough.
        case Status::handshake_hello_received:
            err = try_send_hello_message();
            if (err == ONE_ERROR_TRY_AGAIN) break;
            if (is_error(err)) return fail(err);
            // Assume handshaking is complete now. This side is free to send other
            // Messages now. If handshaking fails on the server, then the connection
            // will be closed and the Messages will be ignored.
            _status = Status::ready;
            break;
        case Status::handshake_hello_scheduled:
            // Ensure nothing is received. Arcus client should not send
            // until it receives a Hello.
            err = ensure_nothing_received();
            if (is_error(err)) return fail(err);

            // Send the hello.
            err = try_send_hello();
            if (err == ONE_ERROR_TRY_AGAIN) break;
            if (is_error(err)) return fail(err);

            _status = Status::handshake_hello_sent;
            break;
        case Status::handshake_hello_sent:
            err = try_receive_hello_message();
            if (err == ONE_ERROR_TRY_AGAIN) break;
            if (is_error(err)) return fail(err);
            _status = Status::ready;
            break;
        default:
            _status = Status::error;
            return fail(ONE_ERROR_CONNECTION_UNKNOWN_STATUS);
    }

    return ONE_ERROR_NONE;
}

Error Connection::process_incoming_messages() {
    codec::Header header = {0};
    auto err = ONE_ERROR_NONE;

    // Attempts to read into the above message Header from the socket and
    // returns true if successful. Sets the above error if an error is
    // encountered.
    auto read_header_and_continue = [&]() -> bool {
        // Check if socket is ready to read anything.
        bool is_ready = false;
        err = _socket.ready_for_read(0.f, is_ready);
        if (is_error(err)) return false;
        if (!is_ready) return false;

        err = try_receive_message_header(header);
        if (err == ONE_ERROR_TRY_AGAIN) {
            err = ONE_ERROR_NONE;
            return false;
        }
        if (is_error(err)) return false;

        return true;
    };

    while (read_header_and_continue()) {
        // Convert to Message.
        auto message = new Message;
        // Todo - json body, if any.
        message->init(static_cast<Opcode>(header.opcode), {nullptr, 0});

        // Store in incoming queue for consumption.
        _incoming_messages.push(message);
    }

    if (is_error(err)) _status = Status::error;

    return err;
}

Error Connection::process_outgoing_messages() {
    // Util to attempt to send all pending data in the buffered outgoing data
    // stream.
    auto send_pending_data = [&]() -> Error {
        size_t size = _out_stream.size();
        if (size == 0) return ONE_ERROR_NONE;

        // Check is socket is connected and ready.
        bool can_send = false;
        auto err = _socket.ready_for_send(0.f, can_send);
        if (is_error(err)) return err;
        if (!can_send) return ONE_ERROR_NONE;

        // Try to send pending data.
        void *data;
        _out_stream.peek(size, &data);
        size_t sent = 0;
        err = _socket.send(data, size, sent);
        if (err == ONE_ERROR_TRY_AGAIN) return ONE_ERROR_NONE;
        if (is_error(err)) return err;

        _out_stream.trim(size);

        return ONE_ERROR_NONE;
    };

    // Send from outgoing buffer if any previous messages are not finished
    // sending.
    auto err = send_pending_data();
    if (is_error(err)) return err;

    // Attempt to send all pending messages.
    bool did_drop_messages = false;
    while (_outgoing_messages.size() > 0) {
        // Convert to data payload and add to outgoing buffer.
        auto message = _outgoing_messages.pop();

        // Prepare data to send, see if it fits in outgoing stream.
        auto payload_pair = message->payload().to_json();
        size_t max_size = _out_stream.capacity() - _out_stream.size();
        size_t message_size = codec::header_size() + payload_pair.second;
        // If it doesn't fit, then put the the connection into an error state.
        if (message_size > max_size) {
            _status = Status::error;
            return ONE_ERROR_CONNECTION_OUT_MESSAGE_TOO_BIG_FOR_STREAM;
        }

        static codec::Header header = {0};
        static uint32_t packet_id = 1;
        header.opcode = static_cast<char>(message->code());
        header.packet_id = packet_id++;
        static std::array<char, codec::header_size()> header_bytes;
        codec::header_to_data(header, header_bytes);
        _out_stream.put(header_bytes.data(), header_bytes.size());

        // Todo - JSON payload.

        err = send_pending_data();
        if (err == ONE_ERROR_TRY_AGAIN) return ONE_ERROR_NONE;
        if (is_error(err)) {
            _status = Status::error;
            return err;
        }
    }

    return ONE_ERROR_NONE;
}

}  // namespace one
