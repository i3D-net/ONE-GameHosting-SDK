#pragma once

#include <mutex>

#include <one/ping/error.h>
#include <one/ping/internal/pinger.h>
#include <one/ping/ip_list.h>
#include <one/ping/logger.h>
#include <one/ping/types.h>

namespace i3d {
namespace ping {

// The i3D Ping Client is used to get the i3D ping latency.
class Pingers final {
public:
    Pingers();
    Pingers(const Pingers &) = delete;
    Pingers &operator=(const Pingers &) = delete;
    ~Pingers();

    void set_logger(const Logger &);

    I3dPingError init(const IpList &ip_list);
    void shutdown();
    I3dPingError update();

    size_t size() const {
        return _pingers.size();
    }

    enum class Status { uninitialized, initialized, unknown };
    static String status_to_string(Status status);

    Status status() const;

    I3dPingError last_time(unsigned int pos, int &duration_ms) const;
    I3dPingError average_time(unsigned int pos, double &duration_ms) const;
    I3dPingError min_time(unsigned int pos, int &duration_ms) const;
    I3dPingError max_time(unsigned int pos, int &duration_ms) const;
    I3dPingError median_time(unsigned int pos, double &duration_ms) const;
    I3dPingError ping_response_count(unsigned int pos,
                                     unsigned int &response_count) const;

    I3dPingError at_least_one_site_has_been_pinged(bool &result) const;
    I3dPingError all_sites_have_been_pinged(bool &result) const;

private:
    I3dPingError update_pings();

    I3dPingError number_sites_pigned(unsigned int &count) const;

    mutable std::mutex _ping;

    Logger _logger;

    Vector<Pinger> _pingers;
    Status _status;
};

}  // namespace ping
}  // namespace i3d
