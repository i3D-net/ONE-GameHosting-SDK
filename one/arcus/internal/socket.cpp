#include <one/arcus/internal/socket.h>

#include <assert.h>
#include <chrono>
#include <cstring>
#include <thread>

#ifdef WINDOWS
typedef int socklen_t;
#else
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include <one/arcus/internal/platform.h>

namespace one {

struct Endianness {
    enum Arch { Big, Little };

    static Arch which() {
        union _ {
            int asInt;
            char asChar[sizeof(int)];
        } u;

        u.asInt = 1;
        return (u.asChar[sizeof(int) - 1] == 1) ? Big : Little;
    }
};

namespace endianness {

enum Type { big, little };

Type which() {
    union {
        uint32_t i;
        char c[4];
    } big_int = {0x01020304};

    return (big_int.c[0] == 1 ? big : little);
}

}  // namespace endianness

Error init_socket_system() {
#ifdef WINDOWS
    WSADATA wsaData;
    const auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err < 0) return ONE_ERROR_SOCKET_SYSTEM_INIT_FAIL;
#endif
    return ONE_ERROR_NONE;
}

Error shutdown_socket_system() {
#ifdef WINDOWS
    const auto result = WSACleanup();
    if (result < 0) {
        return ONE_ERROR_SOCKET_SYSTEM_CLEANUP_FAIL;
    }
#endif
    return ONE_ERROR_NONE;
}

constexpr bool is_error_try_again(int err) {
#ifdef WINDOWS
    return err == WSATRY_AGAIN || err == WSAEWOULDBLOCK;
#else
    return err == EAGAIN;
#endif
}

inline int last_error() {
#ifdef WINDOWS
    return WSAGetLastError();
#else
    return errno;
#endif
}

Socket::Socket() : _socket(INVALID_SOCKET) {}

Socket::~Socket() {
    close();
}

int set_non_blocking(SOCKET &system_socket, bool enable) {
#if defined(WINDOWS)
    u_long as_long = (enable ? 1 : 0);
    return ioctlsocket(system_socket, FIONBIO, &as_long);
#else
    return fcntl(system_socket, F_SETFL, O_NONBLOCK);
#endif
}

Error Socket::init() {
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket == INVALID_SOCKET) {
        return ONE_ERROR_SOCKET_CREATE_FAILED;
    }

#ifndef WINDOWS
    int enable = 1;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        return ONE_ERROR_SOCKET_SOCKET_OPTIONS_FAILED;

    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
        return ONE_ERROR_SOCKET_SOCKET_OPTIONS_FAILED;
#endif

    return ONE_ERROR_NONE;
}

Error Socket::close() {
    if (_socket == INVALID_SOCKET) return ONE_ERROR_NONE;

    // Read until nothing to read.
    char data[1024];
    while (true) {
        int result = ::recv(_socket, data, 1024, 0);
        if (result <= 0) {
            break;
        }
    }

#if defined(WINDOWS)
    const int result = ::closesocket(_socket);
#else
    const int result = ::close(_socket);
#endif

    if (result != 0) return ONE_ERROR_SOCKET_CLOSE_FAILED;

    _socket = INVALID_SOCKET;
    return ONE_ERROR_NONE;
}


Error Socket::bind(const char *ip, unsigned int port) {
    assert(_socket != INVALID_SOCKET);

    sockaddr_in sin;
    sin.sin_port = htons(port);
    if (std::strlen(ip) == 0) {
        sin.sin_addr.s_addr = INADDR_ANY;
    } else {
        sin.sin_addr.s_addr = inet_addr(ip);
    }
    sin.sin_family = AF_INET;
    const int result = ::bind(_socket, (sockaddr *)&sin, sizeof(sin));
    if (result < 0) return ONE_ERROR_SOCKET_BIND_FAILED;
    return ONE_ERROR_NONE;
}

Error Socket::bind(unsigned int port) {
    assert(_socket != INVALID_SOCKET);

    sockaddr_in sin;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_INET;
    const int result = ::bind(_socket, (sockaddr *)&sin, sizeof(sin));
    if (result < 0) return ONE_ERROR_SOCKET_BIND_FAILED;
    return ONE_ERROR_NONE;
}

Error Socket::address(std::string &ip, unsigned int &port) {
    sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);

    const int result = ::getsockname(_socket, (sockaddr *)&addr, &addr_size);
    if (result != 0) return ONE_ERROR_SOCKET_ADDRESS_FAILED;

    assert(addr_size == sizeof(addr));
    ip = std::string(inet_ntoa(addr.sin_addr));
    port = (unsigned int)(ntohs(addr.sin_port));
    return ONE_ERROR_NONE;
}

Error Socket::listen(int queueLength) {
    assert(_socket != INVALID_SOCKET);

    int result = set_non_blocking(_socket, true);
    if (result < 0) return ONE_ERROR_SOCKET_LISTEN_NON_BLOCKING_FAILED;

    result = ::listen(_socket, queueLength);
    if (result < 0) return ONE_ERROR_SOCKET_LISTEN_FAILED;

    return ONE_ERROR_NONE;
}

Error Socket::accept(Socket &client, std::string &ip, unsigned int &port) {
    assert(_socket != INVALID_SOCKET);
    if (client.is_initialized() == true) {
        return ONE_ERROR_SOCKET_ACCEPT_UNINITIALIZED;
    }

    struct sockaddr addr;
    socklen_t addrLen = (socklen_t)sizeof(sockaddr);
    SOCKET socket = ::accept(_socket, &addr, &addrLen);

    // No client is attempting to connect.
    if (socket == INVALID_SOCKET) return ONE_ERROR_NONE;

    // Client received.

    const auto result = set_non_blocking(socket, true);
    if (result < 0) return ONE_ERROR_SOCKET_ACCEPT_NON_BLOCKING_FAILED;

    client._socket = socket;
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    ip = std::string(inet_ntoa(s->sin_addr));
    port = (unsigned int)ntohs(s->sin_port);

    return ONE_ERROR_NONE;
}

Error Socket::connect(const char *ip, const unsigned int port) {
    assert(_socket != INVALID_SOCKET);
    if (std::strlen(ip) == 0) {
        return ONE_ERROR_SOCKET_CONNECT_UNINITIALIZED;
    }

    sockaddr_in sin;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_family = AF_INET;

    int result = ::connect(_socket, (sockaddr *)&sin, sizeof(sin));
    if (result < 0) return ONE_ERROR_SOCKET_CONNECT_FAILED;

    result = set_non_blocking(_socket, true);
    if (result < 0) return ONE_ERROR_SOCKET_CONNECT_NON_BLOCKING_FAILED;

    return ONE_ERROR_NONE;
}

Error Socket::ready_for_read(float timeout, bool &is_ready) {
    is_ready = false;
    if (is_initialized() == false) return ONE_ERROR_SOCKET_SELECT_UNINITIALIZED;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_socket, &fds);

    const auto seconds = (int)(timeout);
    const auto microseconds = (int)((timeout - seconds) * 1000000);
    timeval converted_timeout;
    converted_timeout.tv_sec = seconds;
    converted_timeout.tv_usec = microseconds;

    const int result = ::select((int)_socket + 1, &fds, NULL, NULL, &converted_timeout);
    if (result < 0) return ONE_ERROR_SOCKET_SELECT_FAILED;
    if (result > 0) is_ready = true;
    return ONE_ERROR_NONE;
}

Error Socket::ready_for_send(float timeout, bool &is_ready) {
    is_ready = false;
    if (is_initialized() == false) return ONE_ERROR_SOCKET_SELECT_UNINITIALIZED;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_socket, &fds);

    const auto seconds = (int)(timeout);
    const auto microseconds = (int)((timeout - seconds) * 1000000);
    timeval converted_timeout;
    converted_timeout.tv_sec = seconds;
    converted_timeout.tv_usec = microseconds;

    const int result = ::select((int)_socket + 1, NULL, &fds, NULL, &converted_timeout);
    if (result < 0) return ONE_ERROR_SOCKET_SELECT_FAILED;
    if (result > 0) is_ready = true;
    return ONE_ERROR_NONE;
}

Error Socket::send(const void *data, size_t length, size_t &length_sent) {
    const auto result = ::send(_socket, (const char *)data, length, 0);
    if (result >= 0) {
        length_sent = (size_t)result;
        return ONE_ERROR_NONE;
    }

    length_sent = 0;
    const auto err = last_error();
    if (is_error_try_again(err)) return ONE_ERROR_NONE;
    return ONE_ERROR_SOCKET_SEND_FAILED;
}

Error Socket::available(size_t &length) {
    int result;
#ifdef WINDOWS
    result = ioctlsocket(_socket, FIONREAD, (unsigned long *)&length);
#else
    result = ioctl(_socket, FIONREAD, &length);
#endif
    length = result;
    if (result < 0) return ONE_ERROR_SOCKET_AVAILABLE_FAILED;
    return ONE_ERROR_NONE;
}

Error Socket::receive(void *data, size_t length, size_t &length_received) {
    const auto result = ::recv(_socket, (char *)data, length, 0);
    if (result >= 0) {
        length_received = (size_t)result;
        return ONE_ERROR_NONE;
    }

    length_received = 0;
    const auto err = last_error();
    if (is_error_try_again(err)) return ONE_ERROR_NONE;
    return ONE_ERROR_SOCKET_RECEIVE_FAILED;
}

}  // namespace one
