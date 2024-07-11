#include <one/ping/internal/udp_socket.h>

#include <array>

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
UdpSocket::UdpSocket()
    : _socket(INVALID_SOCKET)
    , _destination{}
    , _source{}
    , _counter(0)
    , _timestamp_send(0)
    , _time_milliseconds(0)
    , _status(Status::uninitialized)
    , _base_data("Hello Arcus ")  // The base data must have a size greather than 2, since
                                  // the ping endpoint mirror the reply with the first two
                                  // byte flipped to '\0'.
{}

I3dPingError UdpSocket::update() {
    I3dPingError err = I3D_PING_ERROR_NONE;

    switch (_status) {
        case Status::uninitialized:
            return I3D_PING_ERROR_SOCKET_NOT_INITIALIZED;
        case Status::initialized:
            err = send_ping();
            if (err == I3D_PING_ERROR_SOCKET_NOT_READY) {
                // Retry until the socket is ready. Without returning an error.
                return I3D_PING_ERROR_NONE;
            } else if (i3d_ping_is_error(err)) {
                _status = Status::error;
                return err;
            }
            break;
        case Status::ping_sent:
            err = receive_ping();
            if (err == I3D_PING_ERROR_SOCKET_NOT_READY) {
                // Retry until the socket is ready. Without returning an error.
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

I3dPingError UdpSocket::init(const char *ipv4, int port) {
    if (ipv4 == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PARAM_IS_NULLPTR;
    }

    if (is_initialized()) {
        return I3D_PING_ERROR_SOCKET_ALREADY_INITIALIZED;
    }

    _ipv4 = ipv4;

#ifdef I3D_PING_WINDOWS
    _socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, 0);
    if (_socket == INVALID_SOCKET) {
        return I3D_PING_ERROR_SOCKET_CREATION_FAIL;
    }

    inet_pton(AF_INET, _ipv4.c_str(), &(_destination.sin_addr));
    _destination.sin_port = htons(port);
    _destination.sin_family = AF_INET;
#else
    _socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_socket == INVALID_SOCKET) {
        return I3D_PING_ERROR_SOCKET_CREATION_FAIL;
    }

    const unsigned int addr = inet_addr(_ipv4.c_str());

    if (addr == INADDR_NONE) {
        return I3D_PING_ERROR_SOCKET_INVALID_IPV4;
    }

    _destination.sin_addr.s_addr = addr;
    _destination.sin_port = htons(port);
    _destination.sin_family = AF_INET;
#endif

    _status = Status::initialized;
    return I3D_PING_ERROR_NONE;
}

I3dPingError UdpSocket::time_milliseconds(int &time_milliseconds) const {
    if (!is_ping_received()) {
        return I3D_PING_ERROR_SOCKET_INVALID_TIME;
    }

    time_milliseconds = _time_milliseconds;
    return I3D_PING_ERROR_NONE;
}

I3dPingError UdpSocket::reset() {
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
}

I3dPingError UdpSocket::send_ping() {
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

    _counter += 1;
    _timestamp_send = get_tick_count();

    _data = _base_data + std::to_string(_counter);

#ifdef I3D_PING_WINDOWS
    // +1 to send the '\0' at the end.
    const int byte_wrote = ::sendto(_socket, (char *)_data.c_str(), _data.size() + 1, 0,
                                    (sockaddr *)&_destination, sizeof(_destination));
    if (byte_wrote == SOCKET_ERROR) {
        return I3D_PING_ERROR_SOCKET_SEND_FAIL;
    }
#else
    // +1 to send the '\0' at the end.
    const int byte_wrote = ::sendto(_socket, (char *)_data.c_str(), _data.size() + 1, 0,
                                    (sockaddr *)&_destination, sizeof(_destination));
    if (byte_wrote < 0) {
        return I3D_PING_ERROR_SOCKET_SEND_FAIL;
    }
#endif

    _status = Status::ping_sent;
    return I3D_PING_ERROR_NONE;
}

I3dPingError UdpSocket::receive_ping() {
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

    std::array<char, 1024> buffer = {'\0'};
    int byte_read = 0;

    // The buffer must be at least bigger than data, to take into acount the trailing
    // '\0'.
    if (buffer.size() < _data.size() + 1) {
        return I3D_PING_ERROR_SOCKET_RECEIVE_BUFFER_TOO_SMALL;
    }

#ifdef I3D_PING_WINDOWS
    int from_length = sizeof(_source);
    byte_read = recvfrom(_socket, buffer.data(), buffer.size(), 0, (sockaddr *)&_source,
                         &from_length);
    if (byte_read == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAEMSGSIZE) {
            return I3D_PING_ERROR_SOCKET_RECEIVE_BUFFER_TOO_SMALL;
        } else {
            return I3D_PING_ERROR_SOCKET_RECEIVE_ERROR;
        }
    }
#else
    unsigned int from_length = sizeof(_source);
    byte_read = recvfrom(_socket, buffer.data(), buffer.size(), 0, (sockaddr *)&_source,
                         &from_length);
    if (byte_read < 0) {
        return I3D_PING_ERROR_SOCKET_RECEIVE_ERROR;
    }
#endif
    // Taking into acount the trailing '\0'.
    if (buffer.size() < _data.size() + 1) {
        return I3D_PING_ERROR_SOCKET_INVALID_REPLY;
    }

    // The ping endpoint will mirror the sent reply with the first two byte changed to
    // '\0'.
    if (buffer[0] != '\0') {
        return I3D_PING_ERROR_SOCKET_INVALID_REPLY;
    }

    if (buffer[1] != '\0') {
        return I3D_PING_ERROR_SOCKET_INVALID_REPLY;
    }

    // Skipping the first two characters since they are '\0'.
    for (unsigned int i = 2; i < _data.size(); ++i) {
        if (buffer[i] != _data[i]) {
            return I3D_PING_ERROR_SOCKET_INVALID_REPLY;
        }
    }

    // Checking the trailing '\0' as it was explicitly sent.
    if (buffer[_data.size()] != '\0') {
        return I3D_PING_ERROR_SOCKET_INVALID_REPLY;
    }

    const unsigned long milleseconds = get_tick_count() - _timestamp_send;

    // Divided by two to take into account the round trip time.
    _time_milliseconds = milleseconds / 2;

    _status = Status::ping_received;
    return I3D_PING_ERROR_NONE;
}

I3dPingError UdpSocket::ready_for_read(int timeout, bool &is_ready) {
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

I3dPingError UdpSocket::ready_for_send(int timeout, bool &is_ready) {
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

bool UdpSocket::is_initialized() const {
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

bool UdpSocket::is_ping_sent() const {
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

bool UdpSocket::is_ping_received() const {
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

unsigned long UdpSocket::get_tick_count() const {
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

}  // namespace ping
}  // namespace i3d
