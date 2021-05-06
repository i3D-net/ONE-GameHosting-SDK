#include <one/ping/internal/sites_endpoint.h>

namespace rapidjson = RAPIDJSON_NAMESPACE;

namespace i3d {
namespace ping {

SitesEndpoint::SitesEndpoint() : _url("https://api.i3d.net/v3/pingsite") {}

I3dPingError SitesEndpoint::parse_payload(const char *json,
                                          Vector<SiteInformation> &sites) const {
    if (json == nullptr) {
        return I3D_PING_ERROR_VALIDATION_JSON_IS_NULLPTR;
    }

    rapidjson::Document doc;
    rapidjson::ParseResult ok = doc.Parse(json);

    if (!ok) {
        return I3D_PING_ERROR_DATA_PARSE_FAILED;
    }

    I3dPingError err = is_payload_valid(doc);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    err = parse_server_information(doc, sites);
    if (i3d_ping_is_error(err)) {
        return err;
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesEndpoint::is_payload_valid(const rapidjson::Value &json) const {
    if (!json.IsArray()) {
        return I3D_PING_ERROR_DATA_JSON_PAYLOAD_IS_INVALID;
    }

    const auto &array = json.GetArray();
    I3dPingError err = I3D_PING_ERROR_NONE;

    for (auto &server_information : array) {
        err = is_server_information_valid(server_information);
        if (i3d_ping_is_error(err)) {
            return err;
        }
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesEndpoint::is_server_information_valid(
    const rapidjson::Value &json) const {
    if (!json.IsObject()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    const auto &info = json.GetObject();

    const auto &continent_id = info.FindMember("continentId");
    if (continent_id == info.MemberEnd()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    if (!continent_id->value.IsInt()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    const auto &country = info.FindMember("country");
    if (country == info.MemberEnd()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    if (!country->value.IsString()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    const auto &dc_location_id = info.FindMember("dcLocationId");
    if (dc_location_id == info.MemberEnd()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    if (!dc_location_id->value.IsInt()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    const auto &dc_location_name = info.FindMember("dcLocationName");
    if (dc_location_name == info.MemberEnd()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    if (!dc_location_name->value.IsString()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    const auto &hostname = info.FindMember("hostname");
    if (hostname == info.MemberEnd()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    if (!hostname->value.IsString()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    const auto &ipv4 = info.FindMember("ipv4");
    if (ipv4 == info.MemberEnd()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    if (!ipv4->value.IsArray()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    const auto &ipv4s = ipv4->value.GetArray();

    for (auto &ip : ipv4s) {
        if (!ip.IsString()) {
            return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
        }
    }

    const auto &ipv6 = info.FindMember("ipv6");
    if (ipv6 == info.MemberEnd()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    if (!ipv6->value.IsArray()) {
        return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
    }

    const auto &ipv6s = ipv6->value.GetArray();

    for (auto &ip : ipv6s) {
        if (!ip.IsString()) {
            return I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID;
        }
    }

    return I3D_PING_ERROR_NONE;
}

I3dPingError SitesEndpoint::parse_server_information(
    const rapidjson::Value &json, Vector<SiteInformation> &sites) const {
    sites.clear();

    I3dPingError err = I3D_PING_ERROR_NONE;

    const auto &array = json.GetArray();
    for (auto &value : array) {
        SiteInformation site;
        err = site.set_continent_id(value.FindMember("continentId")->value.GetInt());
        if (i3d_ping_is_error(err)) {
            return err;
        }
        err = site.set_country(value.FindMember("country")->value.GetString());
        if (i3d_ping_is_error(err)) {
            return err;
        }
        err = site.set_dc_location_id(value.FindMember("dcLocationId")->value.GetInt());
        if (i3d_ping_is_error(err)) {
            return err;
        }
        err = site.set_dc_location_name(
            value.FindMember("dcLocationName")->value.GetString());
        if (i3d_ping_is_error(err)) {
            return err;
        }
        err = site.set_hostname(value.FindMember("hostname")->value.GetString());
        if (i3d_ping_is_error(err)) {
            return err;
        }

        const auto &ipv4s = value.FindMember("ipv4")->value.GetArray();

        for (auto &ip : ipv4s) {
            err = site.push_back_ipv4(ip.GetString());
            if (i3d_ping_is_error(err)) {
                return err;
            }
        }

        const auto &ipv6s = value.FindMember("ipv6")->value.GetArray();
        for (auto &ip : ipv6s) {
            err = site.push_back_ipv6(ip.GetString());
            if (i3d_ping_is_error(err)) {
                return err;
            }
        }

        sites.push_back(site);
    }

    return I3D_PING_ERROR_NONE;
}

}  // namespace ping
}  // namespace i3d