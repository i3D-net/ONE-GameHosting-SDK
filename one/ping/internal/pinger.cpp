#include <one/ping/internal/pinger.h>

#include <algorithm>
#include <limits>

#define I3D_PING_MEDIAN_HISTORY_SIZE 10

namespace i3d {
namespace ping {

Pinger::Pinger()
    : _last_time(-1)
    , _total_time(0)
    , _ping_response_count(0)
    , _min_time(-1)
    , _max_time(-1)
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
        const unsigned int numerical_max = (std::numeric_limits<unsigned int>::max)();
        if (_total_time == numerical_max || _ping_response_count == numerical_max) {
            reset();
        }

        _last_time = time;
        _total_time += time;
        ++_ping_response_count;

        if (_max_time < time) {
            _max_time = time;
        }

        // To avoid edge case just after a reset were min is -1.
        if (_min_time == -1) {
            _min_time = time;
        }

        if (time < _min_time) {
            _min_time = time;
        }

        // To avoid _history to be ever increasing.
        if (_history.size() == I3D_PING_MEDIAN_HISTORY_SIZE) {
            _history.pop_front();
        }

        _history.push_back(time);
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

I3dPingError Pinger::min_time(int &duration_ms) const {
    if (_status != Status::initialized) {
        return I3D_PING_ERROR_PINGER_IS_UNINITIALIZED;
    }

    if (_min_time < 0) {
        return I3D_PING_ERROR_PINGER_INVALID_TIME;
    }

    duration_ms = _min_time;
    return I3D_PING_ERROR_NONE;
}

I3dPingError Pinger::max_time(int &duration_ms) const {
    if (_status != Status::initialized) {
        return I3D_PING_ERROR_PINGER_IS_UNINITIALIZED;
    }

    if (_max_time < 0) {
        return I3D_PING_ERROR_PINGER_INVALID_TIME;
    }

    duration_ms = _max_time;
    return I3D_PING_ERROR_NONE;
}

I3dPingError Pinger::median_time(double &duration_ms) const {
    if (_status != Status::initialized) {
        return I3D_PING_ERROR_PINGER_IS_UNINITIALIZED;
    }

    double median = 0.0;
    auto err = compute_median(_history, median);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    duration_ms = median;
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
    _min_time = -1;
    _max_time = -1;
    _history.clear();
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

I3dPingError Pinger::compute_median(const List<int> &history, double &median_time) const {
    if (_history.empty()) {
        return I3D_PING_ERROR_PINGER_INVALID_TIME;
    }

    Vector<int> sample = {history.cbegin(), history.cend()};
    std::sort(sample.begin(), sample.end());

    // If even, take the mean of the two central values.
    if (sample.size() % 2 == 0) {
        int center_pos = (sample.size() / 2) - 1;
        int center_pos_2 = sample.size() / 2;

        median_time =
            static_cast<double>(sample[center_pos] + sample[center_pos_2]) / 2.0;
    } else {  // If odd, take the central value.
        median_time = sample[sample.size() / 2];
    }

    return I3D_PING_ERROR_NONE;
}

}  // namespace ping
}  // namespace i3d
