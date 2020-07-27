#pragma once

#if defined(WINDOWS)
	#include <winsock2.h>
#else
    typedef int SOCKET;
    #ifndef INVALID_SOCKET
        const int INVALID_SOCKET = -1;
    #endif
#endif

namespace one {

// Must be called before using Socket.
int init_socket_system();

// A limited, cross-platform, low level TCP socket interface.
class Socket
{
public:
    // Creates an uninitialized socket. init must be called.
    Socket();

    // Destroys the socket. Closes active connection, if any.
    ~Socket();

    // Initializes as a TCP socket. Must be called before listen or connect.
    int init();

    bool is_initialized() const {return _socket != INVALID_SOCKET;}

    // Closes active socket, if active.
    int close();

private:
    // Disallowed operations.
    explicit Socket(const Socket &other);
    void operator=(const Socket &other);

    SOCKET _socket;
};

} // namespace one