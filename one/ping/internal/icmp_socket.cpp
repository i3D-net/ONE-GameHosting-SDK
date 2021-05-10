#include <one/ping/internal/icmp_socket.h>

#ifdef I3D_PING_LINUX
    #include <time.h>
#endif

namespace i3d {
namespace ping {

I3dPingError init_socket_system() {
#ifdef I3D_PING_WINDOWS
    WSADATA wsaData;
    const auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err < 0) return I3D_PING_ERROR_SOCKET_SYSTEM_INIT_FAIL;
#endif
    return I3D_PING_ERROR_NONE;
}

I3dPingError shutdown_socket_system() {
#ifdef I3D_PING_WINDOWS
    const auto result = WSACleanup();
    if (result < 0) {
        return I3D_PING_ERROR_SOCKET_SYSTEM_CLEANUP_FAIL;
    }
#endif
    return I3D_PING_ERROR_NONE;
}

// See: https://en.cppreference.com/w/cpp/language/value_initialization
// C++11 Value initialization
IcmpSocket::IcmpSocket()
    : _socket(INVALID_SOCKET)
    , _time_to_live(64)
    , _destination{}
    , _source{}
    , _icmp_header{}
    , _receive_buffer{}
    , _time_milliseconds(0)
    , _status(Status::uninitialized)
    , _current_sequence_number(0) {}

I3dPingError IcmpSocket::update() {
    I3dPingError err = I3D_PING_ERROR_NONE;

    switch (_status) {
        case Status::uninitialized:
            return I3D_PING_ERROR_SOCKET_NOT_INITIALIZED;
        case Status::initialized:
            _current_sequence_number = ++_sequence_number;
            err = send_ping();
            if (err == I3D_PING_ERROR_SOCKET_NOT_READY) {
                // Retry until the socker is ready. Without returning an error.
                return I3D_PING_ERROR_NONE;
            } else if (i3d_ping_is_error(err)) {
                _status = Status::error;
                return err;
            }
            break;
        case Status::ping_sent:
            err = receive_ping();
            if (err == I3D_PING_ERROR_SOCKET_NOT_READY) {
                // Retry until the socker is ready. Without returning an error.
                return I3D_PING_ERROR_NONE;
            } else if (i3d_ping_is_error(err)) {
                _status = Status::error;
                return err;
            }
            break;
        case Status::ping_received:
        case Status::error:
            reset();
            break;
        default:
            return I3D_PING_ERROR_SOCKET_UNKNOWN_STATUS;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError IcmpSocket::init(const char *ipv4) {
    if (ipv4 == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IPV4_IS_NULLPTR;
    }

    if (is_initialized()) {
        return I3D_PING_ERROR_SOCKET_ALREADY_INITIALIZED;
    }

    _ipv4 = ipv4;

#ifdef I3D_PING_WINDOWS
    _socket = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, 0, 0, 0);
    if (_socket == INVALID_SOCKET) {
        return I3D_PING_ERROR_SOCKET_CREATION_FAIL;
    }

    auto error = setsockopt(_socket, IPPROTO_IP, IP_TTL, (const char *)&_time_to_live,
                            sizeof(_time_to_live));
    if (error == SOCKET_ERROR) {
        return I3D_PING_ERROR_SOCKET_TTL_SET_SOCKET_OPTION_FAIL;
    }

    inet_pton(AF_INET, _ipv4.c_str(), &(_destination.sin_addr));

    _destination.sin_family = AF_INET;
#else
    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if (_socket == INVALID_SOCKET) {
        return I3D_PING_ERROR_SOCKET_CREATION_FAIL;
    }

    auto error = setsockopt(_socket, IPPROTO_IP, IP_TTL, (const char *)&_time_to_live,
                            sizeof(_time_to_live));
    if (error != 0) {
        return I3D_PING_ERROR_SOCKET_TTL_SET_SOCKET_OPTION_FAIL;
    }

    const unsigned int addr = inet_addr(_ipv4.c_str());

    if (addr == INADDR_NONE) {
        return I3D_PING_ERROR_SOCKET_INVALID_IPV4;
    }

    _destination.sin_addr.s_addr = addr;
    _destination.sin_family = AF_INET;
#endif

    _status = Status::initialized;
    return I3D_PING_ERROR_NONE;
}

I3dPingError IcmpSocket::time_milliseconds(int &time_milliseconds) const {
    if (!is_ping_received()) {
        return I3D_PING_ERROR_SOCKET_INVALID_TIME;
    }

    time_milliseconds = _time_milliseconds;
    return I3D_PING_ERROR_NONE;
}

I3dPingError IcmpSocket::reset() {
    switch (_status) {
        case Status::ping_received:
        case Status::error:
            _status = Status::initialized;
            return I3D_PING_ERROR_NONE;
        case Status::uninitialized:
        case Status::initialized:
        case Status::ping_sent:
        default:
            return I3D_PING_ERROR_SOCKET_CANNOT_BE_RESET;
    }
    return I3D_PING_ERROR_NONE;
}

void IcmpSocket::init_packet(int seq_no) {
    _icmp_header.type = I3D_PING_ICMP_ECHO_REQUEST;
    _icmp_header.type_sub_code = 0;
    _icmp_header.checksum = 0;
    _icmp_header.id = 0;
    _icmp_header.seq = seq_no;
    _icmp_header.timestamp = get_tick_count();
    _icmp_header.checksum = checksum((unsigned short *)&_icmp_header, sizeof(ICMPHeader));
}

unsigned short IcmpSocket::checksum(unsigned short *buffer, int size) const {
    unsigned long checksum = 0;

    // Sum all the words together, adding the final byte if size is odd
    while (size > 1) {
        checksum += *buffer++;
        size -= sizeof(unsigned short);
    }
    if (size) {
        checksum += *(unsigned char *)buffer;
    }

    // Do a little shuffling
    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);

    // Return the bitwise complement of the resulting mishmash
    return (unsigned short)(~checksum);
}

I3dPingError IcmpSocket::send_ping() {
    if (!is_initialized()) {
        return I3D_PING_ERROR_SOCKET_NOT_INITIALIZED;
    }

    bool is_ready = false;

    auto err = ready_for_send(0, is_ready);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (!is_ready) {
        return I3D_PING_ERROR_SOCKET_NOT_READY;
    }

    init_packet(_current_sequence_number);

#ifdef I3D_PING_WINDOWS
    const int byte_wrote = ::sendto(_socket, (char *)&_icmp_header, sizeof(ICMPHeader), 0,
                                    (sockaddr *)&_destination, sizeof(_destination));
    if (byte_wrote == SOCKET_ERROR) {
        return I3D_PING_ERROR_SOCKET_SEND_FAIL;
    }
#else
    const int byte_wrote = ::sendto(_socket, (char *)&_icmp_header, sizeof(ICMPHeader), 0,
                                    (sockaddr *)&_destination, sizeof(_destination));
    if (byte_wrote < 0) {
        return I3D_PING_ERROR_SOCKET_SEND_FAIL;
    }
#endif

    _status = Status::ping_sent;
    return I3D_PING_ERROR_NONE;
}

I3dPingError IcmpSocket::receive_ping() {
    if (!is_ping_sent()) {
        return I3D_PING_ERROR_SOCKET_PING_NOT_SENT;
    }

    bool is_ready = false;

    auto err = ready_for_read(0, is_ready);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (!is_ready) {
        return I3D_PING_ERROR_SOCKET_NOT_READY;
    }

#ifdef I3D_PING_WINDOWS
    int from_lenght = sizeof(_source);
    int byte_read = recvfrom(_socket, (char *)&_receive_buffer, sizeof(IPHeader), 0,
                             (sockaddr *)&_source, &from_lenght);
    if (byte_read == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAEMSGSIZE) {
            return I3D_PING_ERROR_SOCKET_RECEIVE_BUFFER_TOO_SMALL;
        } else {
            return I3D_PING_ERROR_SOCKET_RECEIVE_ERROR;
        }
    }
#else
    unsigned int from_lenght = sizeof(_source);
    int byte_read = recvfrom(_socket, (char *)&_receive_buffer, sizeof(IPHeader), 0,
                             (sockaddr *)&_source, &from_lenght);
    if (byte_read < 0) {
        return I3D_PING_ERROR_SOCKET_RECEIVE_ERROR;
    }
#endif

    err = decode_reply();
    if (i3d_ping_is_error(err)) {
        _status = Status::error;
        return err;
    }

    _status = Status::ping_received;
    return I3D_PING_ERROR_NONE;
}

I3dPingError IcmpSocket::decode_reply() {
    const int packet_size = sizeof(IPHeader);
    unsigned short header_lenght = _receive_buffer.header_lenght * 4;
    ICMPHeader *icmp_header = (ICMPHeader *)((char *)&_receive_buffer + header_lenght);

    // Make sure the reply is sane
    if (packet_size < header_lenght + I3D_PING_ICMP_MIN) {
        return I3D_PING_ERROR_SOCKET_RECEIVE_TOO_FEW_BYTES;
    } else if (icmp_header->type != I3D_PING_ICMP_ECHO_REPLY) {
        if (icmp_header->type != I3D_PING_ICMP_TTL_EXPIRE) {
            if (icmp_header->type == I3D_PING_ICMP_DEST_UNREACH) {
                return I3D_PING_ERROR_SOCKET_RECEIVE_DESTINATION_UNREACHABLE;
            } else {
                return I3D_PING_ERROR_SOCKET_RECEIVE_UNKNOWN_ICMP_PACKET_TYPE;
            }
        }
        if (icmp_header->type == I3D_PING_ICMP_TTL_EXPIRE) {
            return I3D_PING_ERROR_SOCKET_RECEIVE_TTL_EXPIRED;
        }
    } else if (icmp_header->seq != _current_sequence_number) {
        return I3D_PING_ERROR_SOCKET_INVALID_SEQUENCE_NUMBER;
    }

    const unsigned long milleseconds = get_tick_count() - icmp_header->timestamp;
    _time_milliseconds = milleseconds;
    return I3D_PING_ERROR_NONE;
}

I3dPingError IcmpSocket::ready_for_read(int timeout, bool &is_ready) {
    is_ready = false;

    if (!is_initialized()) {
        return I3D_PING_ERROR_SOCKET_NOT_INITIALIZED;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_socket, &fds);

    const auto seconds = timeout;
    const auto microseconds = (timeout - seconds) * 1000000;
    timeval converted_timeout;
    converted_timeout.tv_sec = seconds;
    converted_timeout.tv_usec = microseconds;

    const int result = ::select((int)_socket + 1, &fds, NULL, NULL, &converted_timeout);
    if (result < 0) return I3D_PING_ERROR_SOCKET_SELECT_FAILED;
    if (result > 0) is_ready = true;
    return I3D_PING_ERROR_NONE;
}

I3dPingError IcmpSocket::ready_for_send(int timeout, bool &is_ready) {
    is_ready = false;

    if (!is_initialized()) {
        return I3D_PING_ERROR_SOCKET_NOT_INITIALIZED;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(_socket, &fds);

    const auto seconds = timeout;
    const auto microseconds = (timeout - seconds) * 1000000;
    timeval converted_timeout;
    converted_timeout.tv_sec = seconds;
    converted_timeout.tv_usec = microseconds;

    const int result = ::select((int)_socket + 1, NULL, &fds, NULL, &converted_timeout);
    if (result < 0) return I3D_PING_ERROR_SOCKET_SELECT_FAILED;
    if (result > 0) is_ready = true;
    return I3D_PING_ERROR_NONE;
}

bool IcmpSocket::is_initialized() const {
    switch (_status) {
        case Status::uninitialized:
            return false;
            ;
        case Status::initialized:
        case Status::ping_sent:
        case Status::ping_received:
        case Status::error:
        default:
            return true;
    }
}

bool IcmpSocket::is_ping_sent() const {
    switch (_status) {
        case Status::ping_sent:
            return true;
            ;
        case Status::uninitialized:
        case Status::initialized:
        case Status::ping_received:
        case Status::error:
        default:
            return false;
    }
}

bool IcmpSocket::is_ping_received() const {
    switch (_status) {
        case Status::ping_received:
            return true;
        case Status::uninitialized:
        case Status::initialized:
        case Status::ping_sent:
        case Status::error:
        default:
            return false;
    }
}

unsigned long IcmpSocket::get_tick_count() const {
    unsigned long count = 0;
#ifdef I3D_PING_WINDOWS
    count = GetTickCount();
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    count = ts.tv_nsec / 1000000;
    count += ts.tv_sec * 1000;
#endif
    return count;
}

unsigned int IcmpSocket::_sequence_number = 0;

}  // namespace ping
}  // namespace i3d
