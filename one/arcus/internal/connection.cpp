#include <one/arcus/internal/connection.h>

#include <assert.h>
#include <cstring>

#include <one/arcus/message.h>
#include <one/arcus/opcode.h>
#include <one/arcus/c_platform.h>
#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/socket.h>

#ifdef ONE_WINDOWS
#else
    #include <errno.h>
#endif

//#define ONE_ARCUS_CONNECTION_LOGGING
#ifdef ONE_ARCUS_CONNECTION_LOGGING
    #include <functional>
    #include <iostream>
#endif

namespace i3d {
namespace one {

#ifdef ONE_ARCUS_CONNECTION_LOGGING
namespace {

void log(const Socket &socket, std::function<void(OStringStream &)> cb) {
    // Get socket info to help identify the connection.
    String ip;
    unsigned int port;
    socket.address(ip, port);
    OStringStream stream;

    // Write it to the stream, then allow caller to add more.
    stream << "ip: " << ip << ", port: " << port << ". ";
    cb(stream);

    std::cout << stream.str() << std::endl;
}

}  // namespace
#endif  // ONE_ARCUS_CONNECTION_LOGGING

Connection::Connection(size_t max_messages_in, size_t max_messages_out)
    : _socket(nullptr)
    , _status(Status::uninitialized)
    , _in_stream(connection::stream_receive_buffer_size())
    , _out_stream(connection::stream_send_buffer_size())
    , _incoming_messages(max_messages_in)
    , _outgoing_messages(max_messages_out)
    , _handshake_timer(handshake_timeout_seconds)
    , _health_checker(HealthChecker::health_check_send_interval_seconds,
                      HealthChecker::health_check_receive_interval_seconds) {
    _handshake_timer.sync_now();
}

void Connection::init(Socket &socket) {
    assert(_status == Status::uninitialized);
    _socket = &socket;
    _handshake_timer.sync_now();
    _health_checker.reset_receive_timer();
    _status = Status::handshake_not_started;
}

void Connection::shutdown() {
    _out_stream.clear();
    _in_stream.clear();
    _outgoing_messages.clear();
    _incoming_messages.clear();
    _status = Status::uninitialized;
    _socket = nullptr;
}

Connection::Status Connection::status() const {
    return _status;
}

OneError Connection::add_outgoing(const Message &message) {
    if (_status == Status::uninitialized) return ONE_ERROR_CONNECTION_UNINITIALIZED;

    if (_outgoing_messages.size() == _outgoing_messages.capacity())
        return ONE_ERROR_CONNECTION_OUTGOING_QUEUE_INSUFFICIENT_SPACE;

    _outgoing_messages.push(message);
    return ONE_ERROR_NONE;
}

OneError Connection::incoming_count(unsigned int &count) const {
    if (_status == Status::uninitialized) return ONE_ERROR_CONNECTION_UNINITIALIZED;

    count = static_cast<unsigned int>(_incoming_messages.size());
    return ONE_ERROR_NONE;
}

OneError Connection::remove_incoming(
    std::function<OneError(const Message &message)> read_callback) {
    assert(read_callback);
    if (_status == Status::uninitialized) return ONE_ERROR_CONNECTION_UNINITIALIZED;

    if (_incoming_messages.size() == 0) {
        return ONE_ERROR_CONNECTION_QUEUE_EMPTY;
    }

    Message &message = _incoming_messages.pop();
    auto err = read_callback(message);
    message.reset();

    return err;
}

OneError Connection::initiate_handshake() {
    if (_status == Status::uninitialized) return ONE_ERROR_CONNECTION_UNINITIALIZED;
    assert(_status == Status::handshake_not_started);
    _status = Status::handshake_hello_scheduled;

    return ONE_ERROR_NONE;
}

OneError Connection::process_health() {
    if (_health_checker.process_receive()) {
        _status = Status::error;
        return ONE_ERROR_CONNECTION_HEALTH_TIMEOUT;
    }
    auto sender = [this](const Message &m) { return add_outgoing(m); };
    auto err = _health_checker.process_send(sender);
    if (is_error(err)) {
        _status = Status::error;
        return err;
    }
    return ONE_ERROR_NONE;
}

OneError Connection::update() {
    if (_status == Status::uninitialized) return ONE_ERROR_CONNECTION_UNINITIALIZED;
    if (_status == Status::error) return ONE_ERROR_CONNECTION_UPDATE_AFTER_ERROR;

    assert(_socket && _socket->is_initialized());

    if (_status == Status::ready) {
        auto err = process_health();
        if (is_error(err)) {
            return err;
        }
    }

#ifdef ONE_ARCUS_CONNECTION_LOGGING
    log(*_socket, [&](OStringStream &stream) { stream << "connection update"; });
#endif

    // Return if socket has no activity or has an error.
    bool is_ready = false;
    auto err = _socket->ready_for_send(0.f, is_ready);
    if (is_error(err)) return err;
    if (!is_ready) return ONE_ERROR_NONE;

    if (_status != Status::ready) return process_handshake();

#ifdef ONE_ARCUS_CONNECTION_LOGGING
    log(*_socket,
        [](OStringStream &stream) { stream << "connection processing messages"; });
#endif

    // The server mostly replies to client request, so send outgoing messages
    // first.
    err = process_outgoing_messages();
    if (is_error(err)) return err;  // Flush incoming also if error on outgoing?
    return process_incoming_messages();
}

OneError Connection::ensure_nothing_received() {
    assert(_socket && _socket->is_initialized());

    char byte;
    size_t received = 0;
    auto err = _socket->receive(&byte, 1, received);
    if (is_error(err)) {
        return err;
    }
    if (received > 0) {
        return ONE_ERROR_CONNECTION_RECEIVE_BEFORE_SEND;
    }
    return ONE_ERROR_NONE;
}

OneError Connection::try_send_hello() {
    assert(_socket && _socket->is_initialized());

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
    auto err = _socket->send(data, size, sent);
    if (is_error(err)) {  // Error.
        return ONE_ERROR_CONNECTION_HELLO_SEND_FAILED;
    }
    if (sent == 0) return ONE_ERROR_CONNECTION_TRY_AGAIN;

    // Remove from send stream, check if finished.
    stream.trim(sent);
    if (stream.size() > 0) return ONE_ERROR_CONNECTION_TRY_AGAIN;

    return ONE_ERROR_NONE;
}

OneError Connection::try_receive_hello() {
    assert(_socket && _socket->is_initialized());

    // Read a hello packet from socket.

    // See: https://en.cppreference.com/w/cpp/language/value_initialization
    // C++11 Value initialization
    codec::Hello hello{};
    size_t received = 0;
    auto err = _socket->receive(&hello, codec::hello_size(), received);
    if (is_error(err)) {
        return ONE_ERROR_CONNECTION_HELLO_RECEIVE_FAILED;
    }
    if (received == 0) {                        // No error but nothing received.
        return ONE_ERROR_CONNECTION_TRY_AGAIN;  // Retry next attempt.
    }
    if (received > codec::hello_size()) {
        return ONE_ERROR_CONNECTION_HELLO_TOO_BIG;
    }

    // Buffer bytes read. This will normally be the entire hello, but
    // there are edge cases that might result in partial reads. Return
    // if the full hello has not been read yet.
    _in_stream.put(&hello, received);
    if (_in_stream.size() < codec::hello_size()) {
        return ONE_ERROR_CONNECTION_TRY_AGAIN;
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
    // See: https://en.cppreference.com/w/cpp/language/value_initialization
    // C++11 Value initialization
    static codec::Header message{};
    message.opcode = static_cast<char>(Opcode::hello);
    return message;
}

OneError Connection::try_send_hello_message() {
    assert(_socket && _socket->is_initialized());

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
    auto err = _socket->send(data, size, sent);
    if (is_error(err)) {
        return ONE_ERROR_CONNECTION_HELLO_MESSAGE_SEND_FAILED;
    }
    if (sent == 0) return ONE_ERROR_CONNECTION_TRY_AGAIN;

    // Remove from send stream, check if finished.
    stream.trim(sent);
    if (stream.size() > 0) return ONE_ERROR_CONNECTION_TRY_AGAIN;

    return ONE_ERROR_NONE;
}

OneError Connection::try_read_data_into_in_stream() {
    assert(_socket && _socket->is_initialized());

    constexpr size_t max_read_size = codec::header_size() + codec::payload_max_size();
    static std::array<char, max_read_size> buffer;
    const size_t available_size = _in_stream.capacity() - _in_stream.size();
    const size_t read_size =
        (max_read_size > available_size) ? available_size : max_read_size;

    size_t received = 0;
    auto err = _socket->receive(buffer.data(), read_size, received);
    if (is_error(err)) {
        _status = Status::error;
        return ONE_ERROR_CONNECTION_MESSAGE_RECEIVE_FAILED;
    }

#ifdef ONE_ARCUS_CONNECTION_LOGGING
    log(*_socket, [&](OStringStream &stream) {
        stream << "connection received data: " << received;
    });
#endif

    if (received > _in_stream.capacity() - _in_stream.size()) {
        _status = Status::error;
        return ONE_ERROR_CONNECTION_READ_TOO_BIG_FOR_STREAM;
    }

    // Buffer bytes read.
    _in_stream.put(buffer.data(), received);
    if (_in_stream.size() < codec::header_size()) {
#ifdef ONE_ARCUS_CONNECTION_LOGGING
        log(*_socket, [&](OStringStream &stream) {
            stream << "stream size smaller than header size: " << _in_stream.size();
        });
#endif
        return ONE_ERROR_CONNECTION_TRY_AGAIN;
    }

    return ONE_ERROR_NONE;
}

OneError Connection::try_read_message_from_in_stream(codec::Header &header,
                                                     Message &message) {
    const size_t in_stream_size = _in_stream.size();
    if (in_stream_size < codec::header_size())
        return ONE_ERROR_CONNECTION_TRY_AGAIN;  // Nothing to read.

    // Get a pointer to the entire input stream.
    void *data = nullptr;
    _in_stream.peek(in_stream_size, &data);
    assert(data != nullptr);

    // Attempt to read a message from it.
    size_t size_read = 0;
    auto err = codec::data_to_message(data, in_stream_size, size_read, header, message);
    if (is_error(err)) {
        if (err == ONE_ERROR_CODEC_DATA_LENGTH_TOO_SMALL_FOR_PAYLOAD) {
            // More reading is needed to be able to read the entire payload.
            err = ONE_ERROR_CONNECTION_TRY_AGAIN;
        }
        return err;
    }
    _in_stream.trim(size_read);

#ifdef ONE_ARCUS_CONNECTION_LOGGING
    log(*_socket, [&](OStringStream &stream) {
        stream << "connection read message opcode: " << (int)message.code();
    });
#endif

    return ONE_ERROR_NONE;
}

OneError Connection::try_receive_hello_message() {
    auto err = try_read_data_into_in_stream();
    if (is_error(err)) return err;

    // See: https://en.cppreference.com/w/cpp/language/value_initialization
    // C++11 Value initialization
    codec::Header header{};
    Message message;
    err = try_read_message_from_in_stream(header, message);
    if (is_error(err)) return err;

    if (std::memcmp(&header, &hello_message(), codec::header_size()) != 0)
        return ONE_ERROR_CONNECTION_HELLO_MESSAGE_REPLY_INVALID;
    if (!message.payload().is_empty())
        return ONE_ERROR_CONNECTION_HELLO_MESSAGE_REPLY_INVALID;
    return ONE_ERROR_NONE;
}

OneError Connection::process_handshake() {
    assert(_socket && _socket->is_initialized());

    if (_handshake_timer.update()) {
        _status = Status::error;
        return ONE_ERROR_CONNECTION_HANDSHAKE_TIMEOUT;
    }

    OneError err;
    auto fail = [this](OneError err) {
        _status = Status::error;
        return err;
    };

    switch (_status) {
        case Status::handshake_not_started:
            // Check if hello received.
            err = try_receive_hello();
            if (err == ONE_ERROR_CONNECTION_TRY_AGAIN) break;
            if (is_error(err)) return fail(err);

            {
                bool is_ready = false;
                err = _socket->ready_for_send(0.f, is_ready);
                if (is_error(err)) return err;
                if (!is_ready) break;
            }
            // Fallthrough.
        case Status::handshake_hello_received:
            err = try_send_hello_message();
            if (err == ONE_ERROR_CONNECTION_TRY_AGAIN) break;
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
            if (err == ONE_ERROR_CONNECTION_TRY_AGAIN) break;
            if (is_error(err)) return fail(err);

            _status = Status::handshake_hello_sent;
            break;
        case Status::handshake_hello_sent:
            err = try_receive_hello_message();
            if (err == ONE_ERROR_CONNECTION_TRY_AGAIN) break;
            if (is_error(err)) return fail(err);
            _status = Status::ready;
            break;
        default:
            _status = Status::error;
            return fail(ONE_ERROR_CONNECTION_UNKNOWN_STATUS);
    }

    return ONE_ERROR_NONE;
}

OneError Connection::process_incoming_messages() {
    assert(_socket && _socket->is_initialized());

    // See: https://en.cppreference.com/w/cpp/language/value_initialization
    // C++11 Value initialization
    codec::Header header{};
    Message message;
    auto err = ONE_ERROR_NONE;

    // Attempts to get data to process from the socket. Sets the above error if an error
    // is encountered.
    auto get_data_and_continue = [&]() -> bool {
        err = try_read_data_into_in_stream();
        if (err == ONE_ERROR_CONNECTION_TRY_AGAIN) {
            err = ONE_ERROR_NONE;
            return false;
        }
        if (is_error(err)) return false;

        return true;
    };

    // Attempts to read message from the incoming data stream and returns
    // true if successful. Sets the above error if an error is encountered.
    auto read_message_and_continue = [&]() -> bool {
        err = try_read_message_from_in_stream(header, message);
        if (err == ONE_ERROR_CONNECTION_TRY_AGAIN) {
            err = ONE_ERROR_NONE;
            return false;
        }
        if (is_error(err)) return false;

        // At this point data has been received from the remote end so update
        // health timer.
        _health_checker.reset_receive_timer();

        return true;
    };

    while (get_data_and_continue()) {
        while (read_message_and_continue()) {
            // Skip health messages, they are consumed internally and do not
            // make it to the queue for public consumption.
            if (message.code() == Opcode::health) {
                continue;
            }

            if (_incoming_messages.size() == _incoming_messages.capacity()) {
                return ONE_ERROR_CONNECTION_INCOMING_QUEUE_INSUFFICIENT_SPACE;
            }

            // Store in incoming queue for consumption.
            _incoming_messages.push(message);
        }
        if (is_error(err)) break;
    }

    if (is_error(err)) _status = Status::error;

    return err;
}

OneError Connection::process_outgoing_messages() {
    assert(_socket && _socket->is_initialized());

    // Util to attempt to send all pending data in the buffered outgoing data
    // stream.
    auto send_pending_data = [&]() -> OneError {
        size_t size = _out_stream.size();
        if (size == 0) return ONE_ERROR_NONE;

        // Check is socket is connected and ready.
        bool can_send = false;
        auto err = _socket->ready_for_send(0.f, can_send);
        if (is_error(err)) return err;
        if (!can_send) return ONE_ERROR_NONE;

        // Try to send pending data.
        void *data;
        _out_stream.peek(size, &data);
        size_t sent = 0;
        err = _socket->send(data, size, sent);
        if (err == ONE_ERROR_CONNECTION_TRY_AGAIN) return ONE_ERROR_NONE;
        if (is_error(err)) return err;

        _out_stream.trim(size);

#ifdef ONE_ARCUS_CONNECTION_LOGGING
        log(*_socket,
            [&](OStringStream &stream) { stream << "connection sent data: " << sent; });
#endif

        return ONE_ERROR_NONE;
    };

    // Send from outgoing buffer if any previous messages are not finished
    // sending.
    auto err = send_pending_data();
    if (is_error(err)) return err;

#ifdef ONE_ARCUS_CONNECTION_LOGGING
    log(*_socket, [&](OStringStream &stream) {
        stream << "processing outgoing messages: " << _outgoing_messages.size();
    });
#endif

    // Attempt to send all pending messages.
    while (_outgoing_messages.size() > 0) {
        // Convert to data payload and add to outgoing buffer.
        auto message = _outgoing_messages.pop();

        auto fail = [&](OneError err) {
            _status = Status::error;
            return err;
        };

        size_t message_size = 0;
        static uint32_t packet_id = 1;
        static std::array<char, codec::header_size() + codec::payload_max_size()>
            out_message_buffer;
        err =
            codec::message_to_data(packet_id, message, message_size, out_message_buffer);

        if (is_error(err)) {
            return fail(err);
        }

        const size_t max_size = _out_stream.capacity() - _out_stream.size();

        // If it doesn't fit, then put the the connection into an error state.
        if (message_size > max_size) {
            return fail(ONE_ERROR_CONNECTION_OUT_MESSAGE_TOO_BIG_FOR_STREAM);
        }

        _out_stream.put(out_message_buffer.data(), message_size);

        // Incrementing packet_id only after the message has been queued.
        ++packet_id;

        err = send_pending_data();
        if (err == ONE_ERROR_CONNECTION_TRY_AGAIN) return ONE_ERROR_NONE;
        if (is_error(err)) {
            return fail(err);
        }

#ifdef ONE_ARCUS_CONNECTION_LOGGING
        log(*_socket, [&](OStringStream &stream) {
            stream << "connection sent message opcode: " << (int)message.code();
            ;
        });
#endif
    }

    return ONE_ERROR_NONE;
}

}  // namespace one
}  // namespace i3d
