#include <one/arcus/internal/connection.h>

#include <assert.h>
#include <cstring>

#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/opcodes.h>
#include <one/arcus/internal/socket.h>

#ifdef WINDOWS
#else
    #include <errno.h>
#endif

namespace one {

Connection::Connection(Socket& socket, size_t max_messages_in, size_t max_messages_out)
    : _status(Status::handshake_not_started), _socket(socket) {
    assert(socket.is_initialized());
}

bool is_error_try_again(int err) {
#ifdef WINDOWS
    return err == WSATRY_AGAIN || err == WSAEWOULDBLOCK;
#else
    return err == EAGAIN;
#endif
}

int Connection::send(const void* data, size_t length) {
    const auto result = _socket.send(data, length);
    if (result >= 0) {
        return result;
    }
    const auto err = _socket.last_error();
    if (is_error_try_again(err)) return 0;
    _status = Status::error;
    return -1;
}

int Connection::receive(void* data, size_t length) {
    const auto result = _socket.receive(data, length);
    if (result >= 0) {
        return result;
    }
    const auto err = _socket.last_error();
    if (is_error_try_again(err)) return 0;
    _status = Status::error;
    return -1;
}

Connection::Status Connection::status() const {
    return _status;
}

int Connection::push_outgoing(const Message& message) {
    return 0;
}

int Connection::incoming_count() const {
    return 0;
}

int Connection::pop_incoming(Message** message) {
    return 0;
}

void Connection::initiate_handshake() {
    assert(_status == Status::handshake_not_started);
    _status = Status::handshake_hello_scheduled;
}

int Connection::update() {
    if (_status == Status::error) return -1;

    // Return if socket has no activity or has an error.
    const auto select = _socket.select(0.f);
    if (select < 0) return select;

    if (_status != Status::ready) return process_handshake(select > 0);

    return process_messages();
}

void Connection::reset() {
    _status = Status::handshake_not_started;
}

int Connection::process_handshake(bool is_socket_ready) {
    // Check if handshake timed out.
    // return 0;

    // There are two hello packets. The initial codec::hello sent from the
    // handshake initiater, and the response codec::Header message with a
    // hello opcode sent in response. This is the response header.
    static codec::Header hello_header = {0};
    hello_header.opcode = static_cast<char>(Opcodes::hello);

    if (_status == Status::handshake_not_started) {
        // Check if hello received.
        codec::Hello hello = {0};
        auto result = receive(&hello, codec::hello_size());
        if (result < 0) return -1;
        if (result != codec::hello_size()) {
            _status = Status::error;
            return -1;
        }
        if (!codec::validate_hello(hello)) {
            _status = Status::error;
            return -1;
        }

        // Send back a hello Message.
        result = send(&hello_header, codec::header_size());
        if (result < 0) {  // Error.
            return result;
        } else if (result == 0) {  // No error but nothing sent.
            return 0;              // Retry next attempt.
        }

        // Assume handshaking is complete now. This side is free to send other
        // Messages now. If handshaking fails on the server, then the connection
        // will be closed and the Messages will be ignored.
        _status = Status::ready;
    } else if (_status == Status::handshake_hello_scheduled) {
        // Ensure nothing is received. Arcus client should not send
        // until it receives a Hello.
        char byte;
        auto result = receive(&byte, 1);
        if (result != 0) {
            return -1;
        }
        // Send the hello.
        result = send(&codec::valid_hello(), codec::hello_size());
        if (result < 0) {  // Error.
            return result;
        } else if (result == 0) {  // No error but nothing sent.
            return 0;              // Retry next attempt.
        }
        _status = Status::handshake_hello_sent;
    } else if (_status == Status::handshake_hello_sent) {
        // Check if hello message Header received back.
        codec::Header header = {0};
        auto result = receive(&header, codec::header_size());
        if (result < 0) return -1;
        if (result != codec::header_size()) {
            _status = Status::error;
            return -1;
        }
        if (std::memcmp(&header, &hello_header, codec::header_size()) != 0) {
            _status = Status::error;
            return -1;
        }
        _status = Status::ready;
    }

    return 0;
}

int Connection::process_messages() {
    return 0;
}

}  // namespace one
