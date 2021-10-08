#pragma once

#include <one/ping/types.h>
#include <one/ping/error.h>
#include <one/ping/internal/udp_socket.h>

namespace i3d {
namespace ping {

class Pinger final {
public:
    Pinger();
    Pinger(const Pinger &) = default;
    Pinger &operator=(const Pinger &) = default;
    ~Pinger() = default;

    I3dPingError update();

    I3dPingError init(const char *ipv4);

    I3dPingError last_time(int &duration_ms) const;
    I3dPingError average_time(double &duration_ms) const;
    I3dPingError min_time(int &duration_ms) const;
    I3dPingError max_time(int &duration_ms) const;
    I3dPingError median_time(double &duration_ms) const;
    I3dPingError ping_response_count(unsigned int &response_count) const;

    enum class Status { uninitialized, initialized };

    Status status() {
        return _status;
    }

private:
    void reset();

    I3dPingError compute_average(unsigned int total_time, unsigned int response_count,
                                 double &average) const;
    I3dPingError compute_median(const List<int> &history, double &median_time) const;

    UdpSocket _socket;

    int _last_time;
    unsigned int _total_time;
    unsigned int _ping_response_count;
    int _min_time;
    int _max_time;
    List<int> _history;

    Status _status;
};

}  // namespace ping
}  // namespace i3d