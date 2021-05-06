#include <one/ping/pingers.h>

#include <one/ping/allocator.h>
#include <one/ping/internal/icmp_socket.h>

//#define ONE_ARCUS_CLIENT_LOGGING

namespace i3d {
namespace ping {

namespace {
constexpr size_t connection_retry_delay_seconds = 5;
}

// See: https://en.cppreference.com/w/cpp/language/value_initialization
// C++11 Value initialization
Pingers::Pingers() : _status(Status::uninitialized) {}

Pingers::~Pingers() {
    shutdown();
}

void Pingers::set_logger(const Logger &logger) {
    _logger = logger;
}

I3dPingError Pingers::init(const IpList &ip_list) {
    if (_status == Status::initialized) {
        return I3D_PING_ERROR_PINGERS_ALREADY_INITIALIZED;
    }

    auto err = init_socket_system();
    if (i3d_ping_is_error(err)) {
        return err;
    }

    const auto &ips = ip_list.ips();

    _pingers.clear();
    _pingers.resize(ips.size());

    err = I3D_PING_ERROR_NONE;

    for (auto i = 0; i < ips.size(); ++i) {
        err = _pingers[i].init(ips[i].c_str());
        if (i3d_ping_is_error(err)) {
            return err;
        }
    }

    _status = Status::initialized;
    return I3D_PING_ERROR_NONE;
}

void Pingers::shutdown() {
    const std::lock_guard<std::mutex> lock(_ping);
    shutdown_socket_system();
    _pingers.clear();
}

I3dPingError Pingers::update() {
    const std::lock_guard<std::mutex> lock(_ping);

    if (_status == Status::uninitialized) {
        return I3D_PING_ERROR_PINGERS_NOT_INITIALIZED;
    }

    auto err = I3D_PING_ERROR_NONE;

    for (auto &pinger : _pingers) {
        err = pinger.update();
        if (i3d_ping_is_error(err)) {
            return err;
        }
    }

    return I3D_PING_ERROR_NONE;
}

String Pingers::status_to_string(Status status) {
    switch (status) {
        case Status::uninitialized:
            return "uninitialized";
        case Status::initialized:
            return "initialized";
        default:
            return "unknown";
    }
}

Pingers::Status Pingers::status() const {
    const std::lock_guard<std::mutex> lock(_ping);
    return _status;
}

I3dPingError Pingers::last_time(unsigned int pos, int &duration_ms) const {
    const std::lock_guard<std::mutex> lock(_ping);

    if (_pingers.size() <= pos) {
        return I3D_PING_ERROR_PINGERS_POS_IS_OUT_OF_RANGE;
    }

    auto err = _pingers[pos].last_time(duration_ms);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError Pingers::average_time(unsigned int pos, double &duration_ms) const {
    const std::lock_guard<std::mutex> lock(_ping);

    if (_pingers.size() <= pos) {
        return I3D_PING_ERROR_PINGERS_POS_IS_OUT_OF_RANGE;
    }

    auto err = _pingers[pos].average_time(duration_ms);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}
I3dPingError Pingers::ping_response_count(unsigned int pos,
                                          unsigned int &response_count) const {
    const std::lock_guard<std::mutex> lock(_ping);

    if (_pingers.size() <= pos) {
        return I3D_PING_ERROR_PINGERS_POS_IS_OUT_OF_RANGE;
    }

    auto err = _pingers[pos].ping_response_count(response_count);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError Pingers::at_least_one_site_has_been_pinged(bool &result) const {
    const std::lock_guard<std::mutex> lock(_ping);

    unsigned int count = 0;
    auto err = number_sites_pigned(count);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (0 == count) {
        result = false;
    } else {
        result = true;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError Pingers::all_sites_have_been_pinged(bool &result) const {
    const std::lock_guard<std::mutex> lock(_ping);

    unsigned int count = 0;
    auto err = number_sites_pigned(count);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (_pingers.size() != count) {
        result = false;
    } else {
        result = true;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError Pingers::number_sites_pigned(unsigned int &count) const {
    int time = 0;
    I3dPingError err = I3D_PING_ERROR_NONE;

    count = 0;

    for (const auto &ping : _pingers) {
        err = ping.last_time(time);
        if (err == I3D_PING_ERROR_PINGER_INVALID_TIME) {
            continue;
        } else if (i3d_ping_is_error(err)) {
            return err;
        }

        ++count;
    }

    return I3D_PING_ERROR_NONE;
}

}  // namespace ping
}  // namespace i3d
