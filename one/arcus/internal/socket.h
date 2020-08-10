#pragma once

#include <string>

#if defined(WINDOWS)
    #include <winsock2.h>
#else
typedef int SOCKET;
    #ifndef INVALID_SOCKET
const int INVALID_SOCKET = -1;
    #endif
#endif

namespace one {

// Must be called before using Socket. Safe to call multiple times. Calls to
// init_socket_system must have matching calls to shutdown_socket_system.
// The first init call does the initializing. Any calls after the first init
// call only serve to increment a counter to track the number of shutdowns
// needed before the actual shutdown is performed.
int init_socket_system();

// Call when finished using sockets. Safe to call multiple times.
// Must have a matching call to init_socket_subsystem, called first. Only the
// final matching shutdown call actually performs cleanup, the preceding
// calls decrement counters matching the number of times init was called.
void shutdown_socket_system();

// A limited, cross-platform, low level TCP socket interface.
class Socket final {
public:
    //------------
    // Life cycle.

    // Creates an uninitialized socket. init must be called.
    Socket();

    // Destroys the socket. Closes active connection, if any.
    ~Socket();

    // Initializes as a TCP socket. Must be called before listen or connect.
    int init();

    bool is_initialized() const {
        return _socket != INVALID_SOCKET;
    }

    // Closes active socket, if active.
    int close();

    // Returns last socket error.
    int last_error();

    //--------
    // Config.

    // Set whether the socket should block. Default is false (blocks).
    int set_non_blocking(bool enable);

    //--------
    // Server.

    // Assigns the given IP and port to the socket. Use "" for any ip address
    // and 0 for any port.
    int bind(const char *ip, unsigned int port);

    // Assigns the port to the socket. Use 0 for any port.
    int bind(unsigned int port);

    // Returns the address of this socket.
    int address(std::string &ip, unsigned int &port);

    // Listens for incoming connections. Socket must have bind called
    // beforehand.
    int listen(int queueLength);

    // Process incoming listen connections. Returns < 0 if an error occurred
    // during processing. If a new client connection was accepted, then the
    // given client socket's IsInitialized will be true.
    int accept(Socket &client, std::string &ip, unsigned int &port);

    //--------
    // Client.

    int connect(const char *ip, const unsigned int port);

    //--------
    // IO.

    // Non-blocking select checks if socket has been changed.
    int select(float timeout);

    // Send data on the socket. Returns negative number on error, or number of
    // bytes sent. A failure because socket is not ready, e.g. due to
    // EAGAIN on Linux, returns 0.
    int send(const void *data, size_t length);

    // Puts number of bytes available for reading into the given length.
    // Returns < 0 on error.
    int available(size_t &length);

    // Receive data into the given buffer. Returns negative number on error, or
    // number of bytes received. A failure because socket is not ready, e.g. due to
    // EAGAIN on Linux, returns 0.
    int receive(void *data, size_t length);

private:
    explicit Socket(const Socket &other) = delete;
    void operator=(const Socket &other) = delete;

    SOCKET _socket;
};

// Sends, but ignores errors due to socket not ready, e.g. EGAIN on linux.
// 0 is returned in those cases.
int lazy_socket_send(Socket &socket, const void *data, size_t length);

// Receives, but ignores errors due to socket not ready, e.g. EGAIN on linux.
// 0 is returned in those cases.
int lazy_socket_receive(Socket &socket, void *data, size_t length);

}  // namespace one
