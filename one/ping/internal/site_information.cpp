#include <one/ping/internal/site_information.h>

namespace i3d {
namespace ping {

SiteInformation::SiteInformation() : _continent_id(0), _dc_location_id(0) {}

I3dPingError SiteInformation::set_continent_id(int continent_id) {
    _continent_id = continent_id;
    return I3D_PING_ERROR_NONE;
}

I3dPingError SiteInformation::set_country(const char *country) {
    if (country == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PARAM_IS_NULLPTR;
    }

    _country = country;
    return I3D_PING_ERROR_NONE;
}

I3dPingError SiteInformation::set_dc_location_id(int dc_location_id) {
    _dc_location_id = dc_location_id;
    return I3D_PING_ERROR_NONE;
}

I3dPingError SiteInformation::set_dc_location_name(const char *dc_location_name) {
    if (dc_location_name == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PARAM_IS_NULLPTR;
    }

    _dc_location_name = dc_location_name;
    return I3D_PING_ERROR_NONE;
}

I3dPingError SiteInformation::set_hostname(const char *hostname) {
    if (hostname == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PARAM_IS_NULLPTR;
    }

    _hostname = hostname;
    return I3D_PING_ERROR_NONE;
}

I3dPingError SiteInformation::push_back_ipv4(const char *ipv4) {
    if (ipv4 == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PARAM_IS_NULLPTR;
    }

    _ipv4.push_back(ipv4);
    return I3D_PING_ERROR_NONE;
}

I3dPingError SiteInformation::push_back_ipv6(const char *ipv6) {
    if (ipv6 == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PARAM_IS_NULLPTR;
    }

    _ipv6.push_back(ipv6);
    return I3D_PING_ERROR_NONE;
}

}  // namespace ping
}  // namespace i3d