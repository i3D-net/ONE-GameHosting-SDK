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

// ICMP packet types
#define I3D_PING_ICMP_ECHO_REPLY 0
#define I3D_PING_ICMP_DEST_UNREACH 3
#define I3D_PING_ICMP_TTL_EXPIRE 11
#define I3D_PING_ICMP_ECHO_REQUEST 8

// Minimum ICMP packet size, in bytes
#define I3D_PING_ICMP_MIN 8

#ifdef _MSC_VER
    // The following two structures need to be packed tightly, but unlike
    // Borland C++, Microsoft C++ does not do this by default.
    #pragma pack(1)
#endif

struct IPHeader {
    unsigned char header_lenght : 4;
    unsigned char version : 4;
    unsigned char type_of_service;
    unsigned short total_lenght;
    unsigned short uid;
    unsigned short flags;
    unsigned char time_to_live;
    unsigned char protocol;
    unsigned short checksum;
    unsigned long source_ip;
    unsigned long destination_ip;
    char buffer[1024];
};

struct ICMPHeader {
    unsigned char type;
    unsigned char type_sub_code;
    unsigned short checksum;
    unsigned short id;
    unsigned short seq;
    unsigned long timestamp;  // not part of ICMP, but we need it
};

#ifdef _MSC_VER
    #pragma pack()
#endif

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

class IcmpSocket final {
public:
    IcmpSocket();
    IcmpSocket(const IcmpSocket &) = default;
    IcmpSocket &operator=(const IcmpSocket &) = default;
    ~IcmpSocket() = default;

    const String &ipv4() const {
        return _ipv4;
    }

    I3dPingError update();

    I3dPingError init(const char *ipv4);

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
    I3dPingError decode_reply();

    I3dPingError ready_for_read(int timeout, bool &is_ready);
    I3dPingError ready_for_send(int timeout, bool &is_ready);

    bool is_initialized() const;
    bool is_ping_sent() const;
    bool is_ping_received() const;

    unsigned long get_tick_count() const;

    String _ipv4;

    SOCKET _socket;
    int _time_to_live;
    sockaddr_in _destination;
    sockaddr_in _source;
    ICMPHeader _icmp_header;
    IPHeader _receive_buffer;

    int _time_milliseconds;
    Status _status;
    unsigned int _current_sequence_number;
    static unsigned int _sequence_number;
};

}  // namespace ping
}  // namespace i3d