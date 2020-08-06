#include <one/arcus/internal/socket.h>

#include <assert.h>
#include <cstring>

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

int init_socket_system() {
#ifdef WINDOWS
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

void shutdown_socket_system() {
#ifdef WINDOWS
    WSACleanup();
#endif
}

Socket::Socket() : _socket(INVALID_SOCKET) {}

Socket::~Socket() {
    close();
}

int Socket::init() {
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket == INVALID_SOCKET) {
        // Todo errors...
        return -1;
    }
    return 0;
}

int Socket::close() {
    if (_socket == INVALID_SOCKET) return 0;

#if defined(WINDOWS)
    int result = ::closesocket(_socket);
#else
    int result = ::close(_socket);
#endif

    if (result != 0) return result;

    _socket = INVALID_SOCKET;
    return 0;
}

int Socket::last_error() {
#ifdef WINDOWS
    return WSAGetLastError();
#else
    return errno;
#endif
}

int Socket::set_non_blocking(bool enable) {
#if defined(WINDOWS)
    u_long as_long = (enable ? 1 : 0);
    return ioctlsocket(_socket, FIONBIO, &as_long);
#else
    return fcntl(_socket, F_SETFL, O_NONBLOCK);
#endif
}

int Socket::bind(const char* ip, unsigned int port) {
    assert(_socket != INVALID_SOCKET);

    sockaddr_in sin;
    sin.sin_port = htons(port);
    if (std::strlen(ip) == 0) {
        sin.sin_addr.s_addr = INADDR_ANY;
    } else {
        sin.sin_addr.s_addr = inet_addr(ip);
    }
    sin.sin_family = AF_INET;
    return ::bind(_socket, (sockaddr*)&sin, sizeof(sin));
}

int Socket::bind(unsigned int port) {
    assert(_socket != INVALID_SOCKET);

    sockaddr_in sin;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_family = AF_INET;
    return ::bind(_socket, (sockaddr*)&sin, sizeof(sin));
}

int Socket::address(std::string& ip, unsigned int& port) {
    sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);

    int result = ::getsockname(_socket, (sockaddr*)&addr, &addr_size);
    if (result != 0) {
        return result;
    }
    assert(addr_size == sizeof(addr));
    ip = std::string(inet_ntoa(addr.sin_addr));
    port = (unsigned int)(ntohs(addr.sin_port));
    return 0;
}

int Socket::listen(int queueLength) {
    assert(_socket != INVALID_SOCKET);

    return ::listen(_socket, queueLength);
}

int Socket::accept(Socket& client, std::string& ip, unsigned int& port) {
    assert(_socket != INVALID_SOCKET);
    if (client.is_initialized() == true) {
        return -1;
    }

    struct sockaddr addr;
    socklen_t addrLen = (socklen_t)sizeof(sockaddr);
    SOCKET socket = ::accept(_socket, &addr, &addrLen);

    if (socket == INVALID_SOCKET) return 0;

    client._socket = socket;
    struct sockaddr_in* s = (struct sockaddr_in*)&addr;
    ip = std::string(inet_ntoa(s->sin_addr));
    port = (unsigned int)ntohs(s->sin_port);

    return 0;
}

int Socket::connect(const char* ip, const unsigned int port) {
    assert(_socket != INVALID_SOCKET);
    if (std::strlen(ip) == 0) {
        return -1;
    }

    sockaddr_in sin;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_family = AF_INET;

    return ::connect(_socket, (sockaddr*)&sin, sizeof(sin));
}

int Socket::select(float timeout) {
    if (is_initialized() == false) return -1;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_socket, &fds);

    const auto seconds = (int)(timeout);
    const auto microseconds = (int)((timeout - seconds) * 1000000);
    timeval converted_timeout;
    converted_timeout.tv_sec = seconds;
    converted_timeout.tv_usec = microseconds;

    return ::select((int)_socket + 1, NULL, &fds, NULL, &converted_timeout);
}

int Socket::send(const void* data, size_t length) {
    return ::send(_socket, (const char*)data, length, 0);
}

int Socket::available(size_t& length) {
    int result;
#ifdef WINDOWS
    result = ioctlsocket(_socket, FIONREAD, (unsigned long*)&length);
#else
    result = ioctl(_socket, FIONREAD, &length);
#endif
    return result;
}

int Socket::receive(void* data, size_t length) {
    return ::recv(_socket, (char*)data, length, 0);
}

}  // namespace one
