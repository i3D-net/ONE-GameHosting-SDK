#include <catch.hpp>

#include <one/ping/error.h>
#include <one/ping/internal/site_information.h>
#include <one/ping/types.h>

using namespace i3d::ping;

TEST_CASE("site information", "[site information]") {
    SiteInformation site_information;

    I3dPingError err = I3D_PING_ERROR_NONE;
    err = site_information.set_continent_id(1);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(site_information.continent_id() == 1);
    err = site_information.set_country("canada");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(site_information.country() == "canada");
    err = site_information.set_dc_location_id(2);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(site_information.dc_location_id() == 2);
    err = site_information.set_dc_location_name("montreal");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(site_information.dc_location_name() == "montreal");
    err = site_information.set_hostname("toto.montreal.qc.ca");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(site_information.hostname() == "toto.montreal.qc.ca");
    REQUIRE(site_information.ipv4().size() == 0);
    err = site_information.push_back_ipv4("192.168.0.1");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(site_information.ipv4().size() == 1);
    REQUIRE(site_information.ipv4()[0] == "192.168.0.1");
    REQUIRE(site_information.ipv6().size() == 0);
    err = site_information.push_back_ipv6("2001:db8:3333:4444:5555:6666:7777:8888");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(site_information.ipv6().size() == 1);
    REQUIRE(site_information.ipv6()[0] == "2001:db8:3333:4444:5555:6666:7777:8888");
}

TEST_CASE("site information invalid set", "[site information]") {
    SiteInformation site_information;

    I3dPingError err = I3D_PING_ERROR_NONE;
    err = site_information.set_country(nullptr);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_COUNTRY_IS_NULLPTR);
    err = site_information.set_dc_location_name(nullptr);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_DC_LOCATION_NAME_IS_NULLPTR);
    err = site_information.set_hostname(nullptr);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_HOSTNAME_IS_NULLPTR);
    err = site_information.push_back_ipv4(nullptr);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_IPV4_IS_NULLPTR);
    err = site_information.push_back_ipv6(nullptr);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_IPV6_IS_NULLPTR);
}