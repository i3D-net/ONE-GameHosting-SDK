#include <one/fake/ping/i3d_sites_getter_wrapper.h>

#include <assert.h>
#include <string>
#include <cstring>

#include <one/ping/c_api.h>
#include <one/ping/c_error.h>
#include <one/fake/ping/log.h>

namespace i3d_ping_integration {
namespace {

// Logger to pass into One.
void log(void *userdata, I3dPingLogLevel level, const char *message) {
    // userdata not used in this wrapper, but could point to instance of a logger
    // instead of a global logger.

    switch (level) {
        case I3D_PING_LOG_LEVEL_INFO: {
            L_INFO(std::string("ONELOG: ") + message);
            break;
        }
        case I3D_PING_LOG_LEVEL_ERROR: {
            L_ERROR(std::string("ONELOG: ") + message);
            break;
        }
        default: {
            L_ERROR(std::string("ONELOG: unknown log level: ") + message);
        }
    }
}

}  // namespace

I3dSitesGetterWrapper::I3dSitesGetterWrapper() : _sites_getter(nullptr) {}

I3dSitesGetterWrapper::~I3dSitesGetterWrapper() {
    shutdown();
}

bool I3dSitesGetterWrapper::init() {
    const std::lock_guard<std::mutex> lock(_wrapper);

    if (_sites_getter != nullptr) {
        L_ERROR("already initialized");
        return false;
    }

    //-----------------------
    // Create the i3D Ping Sites.

    I3dPingError err = i3d_ping_sites_getter_create(&_sites_getter);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    // Set custom logger - optional.
    err = i3d_ping_sites_getter_set_logger(
        _sites_getter, log,
        nullptr);  // null userdata as global log is used.
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    L_INFO("I3dPingSitesWrapper init complete");
    return true;
}

bool I3dSitesGetterWrapper::init_http_callback() {
    const std::lock_guard<std::mutex> lock(_wrapper);

    if (_sites_getter == nullptr) {
        return false;
    }

    I3dPingError err = i3d_ping_sites_getter_init(_sites_getter);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    L_INFO("I3dPingSitesWrapper init with http callback complete");
    return true;
}

void I3dSitesGetterWrapper::shutdown() {
    const std::lock_guard<std::mutex> lock(_wrapper);

    if (_sites_getter == nullptr) {
        return;
    }

    // Free all objects created via the ONE API. This also shuts down the server
    // first, ending any active connection to it.
    i3d_ping_sites_getter_destroy(_sites_getter);
    _sites_getter = nullptr;
}

bool I3dSitesGetterWrapper::update(bool quiet) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    assert(_sites_getter != nullptr);

    I3dPingError err = i3d_ping_sites_getter_update(_sites_getter);
    if (i3d_ping_is_error(err)) {
        if (!quiet) L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

std::string I3dSitesGetterWrapper::status_to_string(Status status) {
    switch (status) {
        case Status::uninitialized:
            return "uninitialized";
        case Status::initialized:
            return "initialized";
        case Status::waiting:
            return "waiting";
        case Status::ready:
            return "ready";
        case Status::error:
            return "error";
        default:
            return "unknown";
    }
}

I3dSitesGetterWrapper::Status I3dSitesGetterWrapper::status() const {
    const std::lock_guard<std::mutex> lock(_wrapper);

    I3dSitesGetterStatus status;
    I3dPingError err = i3d_ping_sites_getter_status(_sites_getter, &status);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return Status::unknown;
    }
    switch (status) {
        case I3D_SITES_GETTER_STATUS_UNINITIALIZED:
            return Status::uninitialized;
        case I3D_SITES_GETTER_STATUS_INITIALIZED:
            return Status::initialized;
        case I3D_SITES_GETTER_STATUS_WAITING:
            return Status::waiting;
        case I3D_SITES_GETTER_STATUS_READY:
            return Status::ready;
        case I3D_SITES_GETTER_STATUS_ERROR:
            return Status::error;
        default:
            return Status::unknown;
    }
}

bool I3dSitesGetterWrapper::set_http_get_callback(
    void (*callback)(const char *url,
                     void (*)(bool success, const char *json, void *parsing_userdata),
                     void *parsing_userdata, void *http_get_metadata),
    void *userdata) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    I3dPingError err =
        i3d_ping_sites_getter_set_http_get_callback(_sites_getter, callback, userdata);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

bool I3dSitesGetterWrapper::sites_size(size_t &size) const {
    const std::lock_guard<std::mutex> lock(_wrapper);

    unsigned int s = 0;
    I3dPingError err = i3d_ping_sites_getter_site_list_size(_sites_getter, &s);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    size = static_cast<size_t>(s);
    return true;
}

bool I3dSitesGetterWrapper::site_continent_id(unsigned int pos, int &continent_id) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    I3dPingError err =
        i3d_ping_sites_getter_list_site_continent_id(_sites_getter, pos, &continent_id);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

bool I3dSitesGetterWrapper::site_country(unsigned int pos, std::string &country) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    country.clear();
    country.reserve(64);

    I3dPingError err =
        i3d_ping_sites_getter_list_site_country(_sites_getter, pos, &(country[0]));
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

bool I3dSitesGetterWrapper::site_dc_location_id(unsigned int pos, int &dc_location_id) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    I3dPingError err = i3d_ping_sites_getter_list_site_dc_location_id(_sites_getter, pos,
                                                                      &dc_location_id);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

bool I3dSitesGetterWrapper::site_dc_location_name(unsigned int pos,
                                                  std::string &dc_location_name) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    dc_location_name.clear();
    dc_location_name.resize(64);
    I3dPingError err = i3d_ping_sites_getter_list_site_dc_location_name(
        _sites_getter, pos, &(dc_location_name[0]));
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

bool I3dSitesGetterWrapper::site_hostname(unsigned int pos, std::string &hostname) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    hostname.clear();
    hostname.resize(64);
    I3dPingError err =
        i3d_ping_sites_getter_list_site_hostname(_sites_getter, pos, &(hostname[0]));
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

bool I3dSitesGetterWrapper::site_ipv4_size(unsigned int pos, size_t &size) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    unsigned int s = 0;
    I3dPingError err = i3d_ping_sites_getter_list_site_ipv4_size(_sites_getter, pos, &s);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    size = static_cast<size_t>(s);
    return true;
}

bool I3dSitesGetterWrapper::site_ipv4(unsigned int pos, unsigned int ip_pos,
                                      std::string &ip) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    ip.clear();
    ip.resize(16);
    I3dPingError err =
        i3d_ping_sites_getter_list_site_ipv4_ip(_sites_getter, pos, ip_pos, &(ip[0]));
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

bool I3dSitesGetterWrapper::site_ipv6_size(unsigned int pos, size_t &size) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    unsigned int s = 0;
    I3dPingError err = i3d_ping_sites_getter_list_site_ipv6_size(_sites_getter, pos, &s);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    size = static_cast<size_t>(s);
    return true;
}

bool I3dSitesGetterWrapper::site_ipv6(unsigned int pos, unsigned int ip_pos,
                                      std::string &ip) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    ip.clear();
    ip.resize(46);
    I3dPingError err =
        i3d_ping_sites_getter_list_site_ipv6_ip(_sites_getter, pos, ip_pos, &(ip[0]));
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

bool I3dSitesGetterWrapper::ipv4_list(I3dIpListPtr ip_list) const {
    const std::lock_guard<std::mutex> lock(_wrapper);

    I3dPingError err = i3d_ping_sites_getter_ipv4_list(_sites_getter, ip_list);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}
bool I3dSitesGetterWrapper::ipv6_list(I3dIpListPtr ip_list) const {
    const std::lock_guard<std::mutex> lock(_wrapper);

    I3dPingError err = i3d_ping_sites_getter_ipv6_list(_sites_getter, ip_list);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    return true;
}

}  // namespace i3d_ping_integration
