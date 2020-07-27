#include "socket.h"

#include <assert.h>
#include <cstring>

#ifdef WINDOWS
    typedef int socklen_t;
#else
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#include "platform.h"

namespace one {

int init_socket_system() {
    #ifdef WINDOWS
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2,2), &wsaData);
    #endif
}

Socket::Socket():
    _socket(INVALID_SOCKET)
{
}

Socket::~Socket()
{
    close();
}

int Socket::init()
{
    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket == INVALID_SOCKET)
    {
        // Todo errors...
        return -1;
    }
    return 0;
}

int Socket::close()
{
    if (_socket == INVALID_SOCKET)
        return 0;

    #if defined(WINDOWS)
        int result = closesocket(_socket);
    #else
        int result = ::close(_socket);
    #endif

    if (result != 0)
        return result;

    _socket = INVALID_SOCKET;
    return 0;
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

int Socket::address(std::string &ip, unsigned int &port) {
    sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);

    int result = ::getsockname(_socket, (sockaddr *)&addr, &addr_size);
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

int Socket::accept(Socket &client, std::string &ip, unsigned int &port) {
    assert(_socket != INVALID_SOCKET);
    if (client.is_initialized() == true) {
        return -1;
    }

    struct sockaddr addr;
    socklen_t addrLen = (socklen_t)sizeof(sockaddr);
    SOCKET socket = ::accept(_socket, &addr, &addrLen);

    if (socket == INVALID_SOCKET) 
        return 0;

    client._socket = socket;
    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
    ip = std::string(inet_ntoa(s->sin_addr));
    port = (unsigned int)ntohs(s->sin_port);

    return 0;
}

int Socket::connect(const char * ip, const unsigned int  port) {
    assert(_socket != INVALID_SOCKET);
    if (std::strlen(ip) == 0) {
        return -1;
    }

    sockaddr_in sin;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_family = AF_INET;

    return ::connect(_socket,(sockaddr*)&sin, sizeof(sin));
}

} // namespace one