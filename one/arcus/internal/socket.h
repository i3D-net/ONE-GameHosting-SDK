#pragma once

#include <one/arcus/platform.h>

#if defined(ONE_WINDOWS)
    #if defined(ONE_UNREAL_WINDOWS)
#include <Windows/AllowWindowsPlatformTypes.h>
    #endif
#include <winsock2.h>
#include <WS2tcpip.h>
    #if defined(ONE_UNREAL_WINDOWS)
#include <Windows/HideWindowsPlatformTypes.h>
    #endif
#else
typedef int SOCKET;
    #ifndef INVALID_SOCKET
const int INVALID_SOCKET = -1;
    #endif
#endif

#include <one/arcus/error.h>
#include <one/arcus/types.h>

namespace i3d {
namespace one {

// Must be called before using Socket. Safe to call multiple times. Calls to
// init_socket_system must have matching calls to shutdown_socket_system.
// The first init call does the initializing. Any calls after the first init
// call only serve to increment a counter to track the number of shutdowns
// needed before the actual shutdown is performed.
Error init_socket_system();

// Call when finished using sockets. Safe to call multiple times.
// Must have a matching call to init_socket_subsystem, called first. Only the
// final matching shutdown call actually performs cleanup, the preceding
// calls decrement counters matching the number of times init was called.
Error shutdown_socket_system();

// A limited, cross-platform, low level TCP socket interface.
class Socket final {
public:
    //------------
    // Life cycle.

    // Creates an uninitialized socket. init must be called.
    Socket();

    // Destroys the socket. Closes active connection, if any.
    ~Socket();

    // Note that the system socket ownership is transferred when sockets are
    // copied or assigned. Be careful when using the socket in data structures,
    // if the socket is automatically copied during use, behavior may be
    // undefined.
    explicit Socket(const Socket &other);
    void operator=(const Socket &other);

    // Initializes as a TCP socket. Must be called before listen or connect.
    Error init();

    bool is_initialized() const {
        return _socket != INVALID_SOCKET;
    }

    // Closes active socket, if active.
    Error close();

    //--------
    // Server.

    // Assigns the given IP and port to the socket. Use "" for any ip address
    // and 0 for any port.
    Error bind(const char *ip, unsigned int port);

    // Assigns the port to the socket. Use 0 for any port.
    Error bind(unsigned int port);

    // Returns the address of this socket.
    Error address(String &ip, unsigned int &port) const;

    // A decent default for the listen queue length for production. Ensure
    // the listen socket is serviced by accept to keep the queue free.
    static constexpr int default_queue_length = 32;

    // Listens for incoming connections. Socket must have bind called
    // beforehand.
    Error listen(int queueLength);

    // Process incoming listen connections. Returns < 0 if an error occurred
    // during processing. If a new client connection was accepted, then the
    // given client socket's IsInitialized will be true.
    Error accept(Socket &client, String &ip, unsigned int &port);

    //--------
    // Client.

    Error connect(const char *ip, const unsigned int port);

    //--------
    // IO.

    // Sets is_ready to true if the socket is ready for reading (accept or receive).
    Error ready_for_read(float timeout, bool &is_ready);

    // Sets is_ready to true if the socket is ready for sending.
    Error ready_for_send(float timeout, bool &is_ready);

    // Sends data on the socket, setting the given length_sent to the number of
    // bytes sent. A failure to due to the socket not being ready, e.g.
    // due to EAGAIN on Linux, is not considered to be an error and returns
    // ONE_ERROR_NONE.
    Error send(const void *data, size_t length, size_t &length_sent);

    // Puts number of bytes available for reading into the given length.
    Error available(size_t &length);

    // Receives data on the socket into the given buffer, setting the given
    // length_received to the number of bytes received. A failure to due to the
    // socket not being ready, e.g. due to EAGAIN on Linux, is not considered
    // to be an error and returns ONE_ERROR_NONE.
    Error receive(void *data, size_t length, size_t &length_received);

private:
    mutable SOCKET _socket;  // Mutable so that the copy constructor and operator can take
                             // ownership of the system socket.
};

}  // namespace one
}  // namespace i3d