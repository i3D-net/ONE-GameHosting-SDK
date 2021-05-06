#include <one/ping/internal/pinger.h>

#include <limits>

namespace i3d {
namespace ping {

Pinger::Pinger()
    : _last_time(-1)
    , _total_time(0)
    , _ping_response_count(0)
    , _status(Status::uninitialized) {}

I3dPingError Pinger::update() {
    if (_status != Status::initialized) {
        return I3D_PING_ERROR_PINGER_IS_UNINITIALIZED;
    }

    int time = 0;

    // If the socket has valid time, we update the average and latest values,
    // because the socket will reset itself automatically on the next update.
    auto err = _socket.time_milliseconds(time);
    if (!i3d_ping_is_error(err)) {
        // Reset the values when the values are too big for unsigned int.
        // https:://stackoverflow.com/questions/27442885/syntax-error-with-stdnumeric-limitsmax
        const unsigned int max = (std::numeric_limits<unsigned int>::max)();
        if (_total_time == max || _ping_response_count == max) {
            reset();
        }

        _last_time = time;
        _total_time += time;
        ++_ping_response_count;
    }

    // The socket is reset after an error.
    err = _socket.update();
    if (i3d_ping_is_error(err)) {
        reset();
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError Pinger::init(const char *ipv4) {
    if (_status == Status::initialized) {
        return I3D_PING_ERROR_PINGER_ALREADY_INITIALIZED;
    }

    if (ipv4 == nullptr) {
        return I3D_PING_ERROR_VALIDATION_IPV4_IS_NULLPTR;
    }

    auto err = _socket.init(ipv4);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    _status = Status::initialized;
    return I3D_PING_ERROR_NONE;
}

I3dPingError Pinger::last_time(int &duration_ms) const {
    if (_status != Status::initialized) {
        return I3D_PING_ERROR_PINGER_IS_UNINITIALIZED;
    }

    if (_last_time < 0) {
        return I3D_PING_ERROR_PINGER_INVALID_TIME;
    }

    duration_ms = _last_time;
    return I3D_PING_ERROR_NONE;
}

I3dPingError Pinger::average_time(double &duration_ms) const {
    if (_status != Status::initialized) {
        return I3D_PING_ERROR_PINGER_IS_UNINITIALIZED;
    }

    double average = 0.0;
    auto err = compute_average(_total_time, _ping_response_count, average);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    duration_ms = average;
    return I3D_PING_ERROR_NONE;
}

I3dPingError Pinger::ping_response_count(unsigned int &response_count) const {
    if (_status != Status::initialized) {
        return I3D_PING_ERROR_PINGER_IS_UNINITIALIZED;
    }

    response_count = _ping_response_count;
    return I3D_PING_ERROR_NONE;
}

void Pinger::reset() {
    _last_time = -1;
    _total_time = 0;
    _ping_response_count = 0;
}

I3dPingError Pinger::compute_average(unsigned int total_time, unsigned int response_count,
                                     double &average) const {
    if (total_time == 0) {
        return I3D_PING_ERROR_PINGER_INVALID_TIME;
    }

    if (response_count == 0) {
        return I3D_PING_ERROR_PINGER_DIVISION_BY_ZERO;
    }

    average = static_cast<double>(total_time) / static_cast<double>(response_count);
    return I3D_PING_ERROR_NONE;
}

}  // namespace ping
}  // namespace i3d
