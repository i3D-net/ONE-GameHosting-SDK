#pragma once

#include <one/ping/c_platform.h>
#include <one/ping/types.h>
#include <one/ping/error.h>

#if defined(I3D_PING_WINDOWS)
    #if defined(I3D_PING_UNREAL_WINDOWS)
        #include <Windows/AllowWindowsPlatformTypes.h>
    #endif
    #include <winsock2.h>
    #include <WS2tcpip.h>
    #if defined(I3D_PING_UNREAL_WINDOWS)
        #include <Windows/HideWindowsPlatformTypes.h>
    #endif
#else
typedef int SOCKET;
    #ifndef INVALID_SOCKET
const int INVALID_SOCKET = -1;
    #endif

    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <sys/ioctl.h>
    #include <unistd.h>

    #include <netinet/tcp.h>

#endif

namespace i3d {
namespace ping {

// Must be called before using Socket. Safe to call multiple times. Calls to
// init_socket_system must have matching calls to shutdown_socket_system.
// The first init call does the initializing. Any calls after the first init
// call only serve to increment a counter to track the number of shutdowns
// needed before the actual shutdown is performed.
I3dPingError init_socket_system();

// Call when finished using sockets. Safe to call multiple times.
// Must have a matching call to init_socket_subsystem, called first. Only the
// final matching shutdown call actually performs cleanup, the preceding
// calls decrement counters matching the number of times init was called.
I3dPingError shutdown_socket_system();

class UdpSocket final {
public:
    UdpSocket();
    UdpSocket(const UdpSocket &) = default;
    UdpSocket &operator=(const UdpSocket &) = default;
    ~UdpSocket() = default;

    const String &ipv4() const {
        return _ipv4;
    }

    I3dPingError update();

    I3dPingError init(const char *ipv4, int port);

    enum class Status { uninitialized, initialized, ping_sent, ping_received, error };

    I3dPingError time_milliseconds(int &time_milliseconds) const;

    Status status() const {
        return _status;
    }

    I3dPingError reset();

private:
    void init_packet(int seq_no);
    unsigned short checksum(unsigned short *buffer, int size) const;
    I3dPingError send_ping();
    I3dPingError receive_ping();

    I3dPingError ready_for_read(int timeout, bool &is_ready);
    I3dPingError ready_for_send(int timeout, bool &is_ready);

    bool is_initialized() const;
    bool is_ping_sent() const;
    bool is_ping_received() const;

    unsigned long get_tick_count() const;

    String _ipv4;

    SOCKET _socket;
    sockaddr_in _destination;
    sockaddr_in _source;

    unsigned long _counter;
    unsigned long _timestamp_send;
    int _time_milliseconds;
    Status _status;

    const std::string _base_data;
    std::string _data;
};

}  // namespace ping
}  // namespace i3d