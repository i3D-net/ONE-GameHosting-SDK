#pragma once

#include <mutex>

#include <one/ping/error.h>
#include <one/ping/logger.h>
#include <one/ping/internal/sites_endpoint.h>
#include <one/ping/internal/site_information.h>
#include <one/ping/ip_list.h>
#include <one/ping/types.h>

namespace i3d {
namespace ping {

struct ClientCallbacks {
    void (*_http_get)(const char *url,
                      void (*)(bool success, const char *json, void *parsing_userdata),
                      void *parsing_userdata, void *http_get_metadata);
    void *_http_get_userdata;
};

// The i3D Ping Client is used to get the i3D ping latency.
class SitesGetter final {
public:
    SitesGetter();
    SitesGetter(const SitesGetter &) = delete;
    SitesGetter &operator=(const SitesGetter &) = delete;
    ~SitesGetter();

    void set_logger(const Logger &);

    I3dPingError init();
    void shutdown();
    I3dPingError update();

    enum class Status { uninitialized, initialized, error, ready };
    static String status_to_string(Status status);

    Status status() const;

    I3dPingError set_http_get_callback(
        void (*callback)(const char *url,
                         void (*)(bool success, const char *json, void *parsing_userdata),
                         void *parsing_userdata, void *http_get_metadata),
        void *userdata);

    size_t sites_size() const;

    I3dPingError site_continent_id(unsigned int pos, int &continent_id);
    I3dPingError site_country(unsigned int pos, String &country);
    I3dPingError site_dc_location_id(unsigned int pos, int &dc_location_id);
    I3dPingError site_dc_location_name(unsigned int pos, String &dc_location_name);
    I3dPingError site_hostname(unsigned int pos, String &hostname);
    I3dPingError site_ipv4_size(unsigned int pos, size_t &size);
    I3dPingError site_ipv4(unsigned int pos, unsigned int ip_pos, String &ip);
    I3dPingError site_ipv6_size(unsigned int pos, size_t &size);
    I3dPingError site_ipv6(unsigned int pos, unsigned int ip_pos, String &ip);

    I3dPingError ipv4_list(IpList &ip_list) const;
    I3dPingError ipv6_list(IpList &ip_list) const;

private:
    I3dPingError get_sites_information();

    static void parsing_callback(bool success, const char *json, void *parsing_userdata);

    I3dPingError site(unsigned int pos, SiteInformation &site) const;
    I3dPingError ipv4(unsigned int pos, unsigned int ip_pos, String &ip) const;
    I3dPingError ipv6(unsigned int pos, unsigned int ip_pos, String &ip) const;

    mutable std::recursive_mutex _sites_getter;

    Logger _logger;

    ClientCallbacks _callbacks;

    SitesEndpoint _endpoint;
    Vector<SiteInformation> _sites;
    Status _status;
};

}  // namespace ping
}  // namespace i3d
