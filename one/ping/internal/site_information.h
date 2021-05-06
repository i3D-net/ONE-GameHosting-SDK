#pragma once

#include <one/ping/types.h>
#include <one/ping/error.h>

namespace i3d {
namespace ping {

class SiteInformation final {
public:
    SiteInformation();
    SiteInformation(const SiteInformation &) = default;
    SiteInformation &operator=(const SiteInformation &) = default;
    ~SiteInformation() = default;

    int continent_id() const {
        return _continent_id;
    }

    I3dPingError set_continent_id(int continent_id);

    const String &country() const {
        return _country;
    }

    I3dPingError set_country(const char *country);

    int dc_location_id() const {
        return _dc_location_id;
    }

    I3dPingError set_dc_location_id(int dc_location_id);

    const String &dc_location_name() const {
        return _dc_location_name;
    }

    I3dPingError set_dc_location_name(const char *dc_location_name);

    const String &hostname() const {
        return _hostname;
    }

    I3dPingError set_hostname(const char *hostname);

    const Vector<String> &ipv4() const {
        return _ipv4;
    }

    I3dPingError push_back_ipv4(const char *ipv4);

    const Vector<String> &ipv6() const {
        return _ipv6;
    }

    I3dPingError push_back_ipv6(const char *ipv6);

private:
    int _continent_id;
    String _country;
    int _dc_location_id;
    String _dc_location_name;
    String _hostname;
    Vector<String> _ipv4;
    Vector<String> _ipv6;
};

}  // namespace ping
}  // namespace i3d