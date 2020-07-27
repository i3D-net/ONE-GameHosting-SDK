#include "socket.h"

#ifdef WINDOWS
#else
//#include <netinet/tcp.h>
//#include <sys/socket.h>
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

Socket::Socket(const Socket &other):
    _socket(INVALID_SOCKET)
{
}

Socket::~Socket()
{
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

} // namespace one
