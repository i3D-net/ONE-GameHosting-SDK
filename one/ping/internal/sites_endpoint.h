#pragma once

#include <one/ping/error.h>
#include <one/ping/internal/rapidjson/document.h>
#include <one/ping/internal/site_information.h>
#include <one/ping/types.h>

namespace rapidjson = RAPIDJSON_NAMESPACE;

namespace i3d {
namespace ping {

class SitesEndpoint final {
public:
    SitesEndpoint();
    SitesEndpoint(const SitesEndpoint &) = delete;
    SitesEndpoint &operator=(const SitesEndpoint &) = delete;
    ~SitesEndpoint() = default;

    const String &url() const {
        return _url;
    }

    I3dPingError parse_payload(const char *json, Vector<SiteInformation> &sites) const;

private:
    I3dPingError is_payload_valid(const rapidjson::Value &json) const;
    I3dPingError is_server_information_valid(const rapidjson::Value &json) const;
    I3dPingError parse_server_information(const rapidjson::Value &json,
                                          Vector<SiteInformation> &sites) const;

    const String _url;
};

}  // namespace ping
}  // namespace i3d