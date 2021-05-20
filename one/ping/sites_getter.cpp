#include <one/ping/sites_getter.h>

#include <one/ping/allocator.h>

//#define ONE_ARCUS_CLIENT_LOGGING

namespace i3d {
namespace ping {

// See: https://en.cppreference.com/w/cpp/language/value_initialization
// C++11 Value initialization
SitesGetter::SitesGetter() : _callbacks{}, _status(Status::uninitialized) {}

SitesGetter::~SitesGetter() {
    shutdown();
}

void SitesGetter::set_logger(const Logger &logger) {
    _logger = logger;
}

I3dPingError SitesGetter::init() {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);

    if (_status != Status::uninitialized) {
        return I3D_PING_ERROR_SITES_GETTER_ALREADY_INITIALIZED;
    }

    if (_callbacks._http_get == nullptr) {
        return I3D_PING_ERROR_SITES_GETTER_HTTP_GET_CALLBACK_NOT_SET;
    }

    _status = Status::initialized;
    return I3D_PING_ERROR_NONE;
}

void SitesGetter::shutdown() {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);

    // See: https://en.cppreference.com/w/cpp/language/value_initialization
    // C++11 Value initialization
    _callbacks = ClientCallbacks{};
}

I3dPingError SitesGetter::update() {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);

    I3dPingError err = I3D_PING_ERROR_NONE;

    switch (_status) {
        case Status::uninitialized:
            return I3D_PING_ERROR_SITES_GETTER_NOT_INITIALIZED;
        case Status::initialized:
            err = get_sites_information();
            if (i3d_ping_is_error(err)) {
                _status = Status::error;
                return err;
            }
            return I3D_PING_ERROR_NONE;
        case Status::waiting:
            // Nothing to do while waiting.
            return I3D_PING_ERROR_NONE;
        case Status::error:
            err = get_sites_information();
            if (i3d_ping_is_error(err)) {
                return err;
            }
            return I3D_PING_ERROR_NONE;
        case Status::ready:
            // Nothing more to do.
            return I3D_PING_ERROR_NONE;
        default:
            return I3D_PING_ERROR_SITES_GETTER_UNKWON_STATUS;
    }
}

String SitesGetter::status_to_string(Status status) {
    switch (status) {
        case Status::uninitialized:
            return "uninitialized";
        case Status::initialized:
            return "initialized";
        case Status::error:
            return "error";
        case Status::ready:
            return "ready";
        default:
            return "unknown";
    }
}

SitesGetter::Status SitesGetter::status() const {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);
    return _status;
}

I3dPingError SitesGetter::set_http_get_callback(
    void (*callback)(const char *url,
                     void (*)(bool success, const char *json, void *parsing_userdata),
                     void *parsing_userdata, void *http_get_metadata),
    void *userdata) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);

    if (callback == nullptr) {
        return I3D_PING_ERROR_VALIDATION_PARAM_IS_NULLPTR;
    }

    _callbacks._http_get = callback;
    _callbacks._http_get_userdata = userdata;
    return I3D_PING_ERROR_NONE;
}

size_t SitesGetter::sites_size() const {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);
    return _sites.size();
}

I3dPingError SitesGetter::site_continent_id(unsigned int pos, int &continent_id) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);
    SiteInformation s;

    auto err = site(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    continent_id = s.continent_id();
    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::site_country(unsigned int pos, String &country) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);
    SiteInformation s;

    auto err = site(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    country = s.country();
    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::site_dc_location_id(unsigned int pos, int &dc_location_id) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);
    SiteInformation s;

    auto err = site(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    dc_location_id = s.dc_location_id();
    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::site_dc_location_name(unsigned int pos,
                                                String &dc_location_name) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);
    SiteInformation s;

    auto err = site(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    dc_location_name = s.dc_location_name();
    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::site_hostname(unsigned int pos, String &hostname) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);
    SiteInformation s;

    auto err = site(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    hostname = s.hostname();
    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::site_ipv4_size(unsigned int pos, size_t &size) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);
    SiteInformation s;

    auto err = site(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    size = s.ipv4().size();
    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::site_ipv4(unsigned int pos, unsigned int ip_pos, String &ip) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);

    auto err = ipv4(pos, ip_pos, ip);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::site_ipv6_size(unsigned int pos, size_t &size) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);
    SiteInformation s;

    auto err = site(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    size = s.ipv6().size();
    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::site_ipv6(unsigned int pos, unsigned int ip_pos, String &ip) {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);

    auto err = ipv6(pos, ip_pos, ip);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::ipv4_list(IpList &ip_list) const {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);

    ip_list.clear();

    auto err = I3D_PING_ERROR_NONE;
    String ip;

    // Only take the first ip of the site.
    for (size_t i = 0; i < _sites.size(); ++i) {
        err = ipv4(i, 0, ip);
        if (i3d_ping_is_error(err)) {
            ip_list.clear();
            return err;
        }

        ip_list.push_back(ip);
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::ipv6_list(IpList &ip_list) const {
    const std::lock_guard<std::recursive_mutex> lock(_sites_getter);

    ip_list.clear();

    auto err = I3D_PING_ERROR_NONE;
    String ip;

    // Only take the first ip of the site.
    for (size_t i = 0; i < _sites.size(); ++i) {
        err = ipv6(i, 0, ip);
        if (i3d_ping_is_error(err)) {
            ip_list.clear();
            return err;
        }

        ip_list.push_back(ip);
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::get_sites_information() {
    if (_callbacks._http_get == nullptr) {
        return I3D_PING_ERROR_SITES_GETTER_CALLBACK_IS_NULLPTR;
    }

    _status = Status::waiting;
    _callbacks._http_get(_endpoint.url().c_str(), SitesGetter::parsing_callback, this,
                         _callbacks._http_get_userdata);
    return I3D_PING_ERROR_NONE;
}

void SitesGetter::parsing_callback(bool success, const char *json,
                                   void *parsing_userdata) {
    if (parsing_userdata == nullptr) {
        return;
    }

    auto sites_getter = reinterpret_cast<SitesGetter *>(parsing_userdata);
    if (sites_getter == nullptr) {
        return;
    }

    if (!success) {
        sites_getter->_status = Status::error;
        return;
    }

    if (json == nullptr) {
        sites_getter->_status = Status::error;
        return;
    }

    const std::lock_guard<std::recursive_mutex> lock(sites_getter->_sites_getter);

    auto &sites = sites_getter->_sites;
    I3dPingError err = sites_getter->_endpoint.parse_payload(json, sites);
    if (i3d_ping_is_error(err)) {
        sites_getter->_status = Status::error;
        return;
    }

    sites_getter->_status = Status::ready;
}

I3dPingError SitesGetter::site(unsigned int pos, SiteInformation &site) const {
    if (_sites.size() <= pos) {
        return I3D_PING_ERROR_PING_SITE_POS_OUT_OF_RANGE;
    }

    site = _sites[pos];
    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::ipv4(unsigned int pos, unsigned int ip_pos, String &ip) const {
    SiteInformation s;
    auto err = site(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (s.ipv4().size() <= ip_pos) {
        return I3D_PING_ERROR_PING_SITE_IP_POS_OUT_OF_RANGE;
    }

    ip = s.ipv4()[ip_pos];
    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesGetter::ipv6(unsigned int pos, unsigned int ip_pos, String &ip) const {
    SiteInformation s;
    auto err = site(pos, s);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    if (s.ipv6().size() <= ip_pos) {
        return I3D_PING_ERROR_PING_SITE_IP_POS_OUT_OF_RANGE;
    }

    ip = s.ipv6()[ip_pos];
    return I3D_PING_ERROR_NONE;
}

}  // namespace ping
}  // namespace i3d
