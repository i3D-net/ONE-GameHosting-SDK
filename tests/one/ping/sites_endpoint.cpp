#include <catch.hpp>

#include <one/ping/error.h>
#include <one/ping/internal/sites_endpoint.h>
#include <one/ping/internal/site_information.h>
#include <one/ping/types.h>

#include <string>

using namespace i3d::ping;

TEST_CASE("ping site endpoint url", "[ping site endpoint]") {
    SitesEndpoint endpoint;

    REQUIRE(!endpoint.url().empty());
    REQUIRE(endpoint.url() == "https://api.i3d.net/v3/pingsite");
}

TEST_CASE("ping site endpoint parse payload", "[ping site endpoint]") {
    // A linted example.
    // [{
    // 	"continentId": 5,
    // 	"country": "Netherlands",
    // 	"dcLocationId": 6,
    // 	"dcLocationName": "i3d-eu-west-1",
    // 	"hostname": "nlrtm1-pingbeacon1.sys.i3d.network",
    // 	"ipv4": ["213.163.66.59"],
    // 	"ipv6": []
    // }, {
    // 	"continentId": 7,
    // 	"country": "United States (USA)",
    // 	"dcLocationId": 1,
    // 	"dcLocationName": "i3d-us-east-1",
    // 	"hostname": "usqas1-pingbeacon1.sys.i3d.network",
    // 	"ipv4": ["162.244.54.59"],
    // 	"ipv6": []
    // }, {
    // ...
    // }]

    const std::string json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]},{\"continentId\":7,"
        "\"country\":\"United States "
        "(USA)\",\"dcLocationId\":1,\"dcLocationName\":\"i3d-us-east-1\",\"hostname\":"
        "\"usqas1-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"162.244.54.59\"],\"ipv6\":[]}"
        ",{\"continentId\":7,\"country\":\"United States "
        "(USA)\",\"dcLocationId\":2,\"dcLocationName\":\"i3d-us-west-1\",\"hostname\":"
        "\"uslax1-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"162.244.52.59\"],\"ipv6\":[]}"
        ",{\"continentId\":8,\"country\":\"Brazil\",\"dcLocationId\":17,"
        "\"dcLocationName\":\"i3d-sa-east-1\",\"hostname\":\"brcoa1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"185.50.104.59\"],\"ipv6\":[]},{\"continentId\":3,"
        "\"country\":\"Hong "
        "Kong\",\"dcLocationId\":9,\"dcLocationName\":\"i3d-ap-east-1\",\"hostname\":"
        "\"hkhkg1-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"43.239.136.59\"],\"ipv6\":[]}"
        ",{\"continentId\":7,\"country\":\"United States "
        "(USA)\",\"dcLocationId\":3,\"dcLocationName\":\"i3d-us-southcentral-1\","
        "\"hostname\":\"usdal1-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"138.128.136."
        "59\"],\"ipv6\":[]},{\"continentId\":7,\"country\":\"United States "
        "(USA)\",\"dcLocationId\":4,\"dcLocationName\":\"i3d-us-northcentral-1\","
        "\"hostname\":\"uschi1-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"138.128.140."
        "59\"],\"ipv6\":[]},{\"continentId\":3,\"country\":\"Singapore\","
        "\"dcLocationId\":10,\"dcLocationName\":\"i3d-ap-southeast-1\",\"hostname\":"
        "\"sgsin1-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"213.179.200.59\"],\"ipv6\":[]"
        "},{\"continentId\":6,\"country\":\"United Arab "
        "Emirates\",\"dcLocationId\":16,\"dcLocationName\":\"i3d-me-south-1\","
        "\"hostname\":\"aedxb1-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"185.179.202."
        "59\"],\"ipv6\":[]},{\"continentId\":5,\"country\":\"Germany\",\"dcLocationId\":"
        "113,\"dcLocationName\":\"i3d-eu-west-2\",\"hostname\":\"defra3-pingbeacon1.sys."
        "i3d.network\",\"ipv4\":[\"188.122.68.59\"],\"ipv6\":[]},{\"continentId\":1,"
        "\"country\":\"South "
        "Africa\",\"dcLocationId\":18,\"dcLocationName\":\"i3d-af-south-1\",\"hostname\":"
        "\"zajnb1-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"185.179.200.59\"],\"ipv6\":[]"
        "},{\"continentId\":3,\"country\":\"Japan\",\"dcLocationId\":8,"
        "\"dcLocationName\":\"i3d-ap-northeast-1\",\"hostname\":\"jptyo1-pingbeacon1.sys."
        "i3d.network\",\"ipv4\":[\"103.194.166.91\"],\"ipv6\":[]},{\"continentId\":4,"
        "\"country\":\"Australia\",\"dcLocationId\":13,\"dcLocationName\":\"i3d-ap-"
        "southeast-2\",\"hostname\":\"ausyd1-pingbeacon1.sys.i3d.network\",\"ipv4\":["
        "\"109.200.215.219\"],\"ipv6\":[]},{\"continentId\":5,\"country\":\"Poland\","
        "\"dcLocationId\":117,\"dcLocationName\":\"i3d-eu-central-1\",\"hostname\":"
        "\"plwaw2-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"188.122.80.59\"],\"ipv6\":[]}"
        ",{\"continentId\":5,\"country\":\"Russian "
        "Federation\",\"dcLocationId\":7,\"dcLocationName\":\"i3d-eu-east-1\","
        "\"hostname\":\"rumow1-pingbeacon1.sys.i3d.network\",\"ipv4\":[\"188.122.82.59\"]"
        ",\"ipv6\":[]}]";

    Vector<SiteInformation> sites;
    SitesEndpoint endpoint;
    I3dPingError err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(sites.size() == 15);
    const SiteInformation &first = sites[0];
    REQUIRE(first.continent_id() == 5);
    REQUIRE(first.country() == "Netherlands");
    REQUIRE(first.dc_location_id() == 6);
    REQUIRE(first.dc_location_name() == "i3d-eu-west-1");
    REQUIRE(first.hostname() == "nlrtm1-pingbeacon1.sys.i3d.network");
    auto &first_ipv4s = first.ipv4();
    REQUIRE(first_ipv4s.size() == 1);
    auto &first_ip = first_ipv4s[0];
    REQUIRE(first_ip == "213.163.66.59");
    auto &first_ipv6s = first.ipv6();
    REQUIRE(first_ipv6s.size() == 0);

    const SiteInformation &last = sites[14];
    REQUIRE(last.continent_id() == 5);
    REQUIRE(last.country() == "Russian Federation");
    REQUIRE(last.dc_location_id() == 7);
    REQUIRE(last.dc_location_name() == "i3d-eu-east-1");
    REQUIRE(last.hostname() == "rumow1-pingbeacon1.sys.i3d.network");
    auto &last_ipv4s = last.ipv4();
    REQUIRE(last_ipv4s.size() == 1);
    auto &last_ip = last_ipv4s[0];
    REQUIRE(last_ip == "188.122.82.59");
    auto &last_ipv6s = last.ipv6();
    REQUIRE(last_ipv6s.size() == 0);
}

TEST_CASE("ping site endpoint parse empty payload", "[ping site endpoint]") {
    std::string json = "[]";

    Vector<SiteInformation> sites;
    sites.push_back(SiteInformation());
    REQUIRE(sites.size() == 1);
    SitesEndpoint endpoint;
    I3dPingError err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(sites.size() == 0);
}

TEST_CASE("ping site endpoint parse invalid payload", "[ping site endpoint]") {
    // A linted example.
    // [{
    // 	"continentId": 5,
    // 	"country": "Netherlands",
    // 	"dcLocationId": 6,
    // 	"dcLocationName": "i3d-eu-west-1",
    // 	"hostname": "nlrtm1-pingbeacon1.sys.i3d.network",
    // 	"ipv4": ["213.163.66.59"],
    // 	"ipv6": []
    // }, {
    // 	"continentId": 7,
    // 	"country": "United States (USA)",
    // 	"dcLocationId": 1,
    // 	"dcLocationName": "i3d-us-east-1",
    // 	"hostname": "usqas1-pingbeacon1.sys.i3d.network",
    // 	"ipv4": ["162.244.54.59"],
    // 	"ipv6": []
    // }, {
    // d::string json = "";

    Vector<SiteInformation> sites;
    SitesEndpoint endpoint;
    I3dPingError err = endpoint.parse_payload(nullptr, sites);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_JSON_IS_NULLPTR);
    std::string json = "{";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_PARSE_FAILED);
    json = "{}";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_PAYLOAD_IS_INVALID);
    json = "[[]]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"_continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":\"5\",\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"_country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":0,\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);

    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"_dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":\"6\","
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"_dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":0,\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"_hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":0,\"ipv4\":[\"213.163.66.59\"]"
        ",\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"_ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":0,\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[0],\"ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"_ipv6\":[]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":0}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[0]}]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);
    json =
        "[{\"continentId\":5,\"country\":\"Netherlands\",\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-eu-west-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d."
        "network\",\"ipv4\":[\"213.163.66.59\"],\"ipv6\":[]}, 0]";
    err = endpoint.parse_payload(json.c_str(), sites);
    REQUIRE(err == I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID);

    REQUIRE(sites.size() == 0);
}
