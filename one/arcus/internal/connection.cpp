#include "connection.h"

#include <assert.h>

#include "codec.h"
#include "socket.h"

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
    const auto result = _socket.send(data, codec::hello_size());
    if (result >= 0) {
        return result;
    }
    const auto err = _socket.last_error();
    if (is_error_try_again(err)) return 0;
    _status = Status::error;
    return -1;
}

Connection::Status Connection::status() const { return _status; }

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

int Connection::process_handshake(bool is_socket_ready) {
    // Check if handshake timed out.
    // return 0;

    if (_status == Status::handshake_not_started) {
        // Check if hello received.
        codec::Hello hello = {0};
        auto result = _socket.receive(&hello, codec::hello_size());
        if (result < 0 || result != codec::hello_size()) {
            _status = Status::error;
            return -1;
        }
        if (!codec::validate_hello(hello)) {
            _status = Status::error;
            return -1;
        }

        // Todo: send back a hello Message.

        // Assume handshaking is complete now. This side is free to send other
        // Messages now. If handshaking fails on the server, then the connection
        // will be closed and the Messages will be ignored.
        _status = Status::ready;
    } else if (_status == Status::handshake_hello_scheduled) {
        const void* hello_data = codec::hello_data();
        auto result = send(hello_data, codec::hello_size());
        if (result < 0) {  // Error.
            return result;
        } else if (result == 0) {  // No error but nothing sent.
            return 0;              // Retry next attempt.
        }
        _status = Status::handshake_hello_sent;
    } else if (_status == Status::handshake_hello_sent) {
    }

    return 0;
}

int Connection::process_messages() { return 0; }

}  // namespace one