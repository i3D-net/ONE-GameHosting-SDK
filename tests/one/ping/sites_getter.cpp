#include <catch.hpp>

#include <curl/curl.h>

#include <one/ping/c_api.h>
#include <one/ping/error.h>

#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <string>
#include <vector>

namespace {
std::size_t callback(const char *in, std::size_t size, std::size_t num,
                     std::string *out) {
    const std::size_t total_Bytes = size * num;
    out->append(in, total_Bytes);
    return total_Bytes;
}

void http_callback(const char *url,
                   void (*parsing_callback)(bool success, const char *json,
                                            void *parsing_userdata),
                   void *parsing_userdata, void *http_get_metadata) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    REQUIRE(curl);

    // Avoid SSL certificate issues.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    // Avoid SSL certificate issues.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    // Don't wait forever, time out after 10 seconds.
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    // Follow HTTP redirects if necessary.
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // Set HTTP url.
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Hook up data handling function.
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

    std::unique_ptr<std::string> http_data(new std::string());
    // Hook up data container (will be passed as the last parameter to the
    // callback handling function).  Can be any pointer type, since it will
    // internally be passed as a void pointer.
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, http_data.get());

    res = curl_easy_perform(curl);
    REQUIRE(res == CURLE_OK);

    long http_code(0);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    bool success = http_code == 200 ? true : false;
    REQUIRE(!http_data.get()->empty());

    parsing_callback(success, http_data.get()->c_str(), parsing_userdata);
}

void test_callback(const char *url,
                   void (*parsing_callback)(bool success, const char *json,
                                            void *parsing_userdata),
                   void *parsing_userdata, void *http_get_metadata) {

    const std::string json =
        "[{\"continentId\":5,\"country\":"
        "\"CanadaCanadaCanadaCanadaCanadaCanadaCanadaCanadaCanadaCanadaCanada\","
        "\"dcLocationId\":6,"
        "\"dcLocationName\":\"i3d-us-east-1i3d-us-east-1i3d-us-east-1i3d-us-east-1i3d-us-"
        "east-1\",\"hostname\":\"nlrtm1-pingbeacon1.sys.i3d.networknlrtm1-pingbeacon1."
        "sys.i3d.network\",\"ipv4\":[\"213.163.660.5900\"],\"ipv6\":[\"2001:0db8:0000:"
        "0000:"
        "0000:0000:1428:57ab:2001:0db8:0000:0000:0000:0000:1428:57ab\"]}]";

    parsing_callback(true, json.c_str(), parsing_userdata);
}

}  // namespace

TEST_CASE("ping sites using libcurl get http callback", "[sites_getter]") {
    I3dSitesGetterPtr sites_getter = nullptr;
    I3dPingError err = I3D_PING_ERROR_NONE;

    i3d_ping_sites_getter_create(&sites_getter);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    err =
        i3d_ping_sites_getter_set_http_get_callback(sites_getter, http_callback, nullptr);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = i3d_ping_sites_getter_init(sites_getter);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = i3d_ping_sites_getter_update(sites_getter);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    I3dSitesGetterStatus status;
    err = i3d_ping_sites_getter_status(sites_getter, &status);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(status == I3D_SITES_GETTER_STATUS_READY);
    unsigned int size = 0;
    err = i3d_ping_sites_getter_site_list_size(sites_getter, &size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(size == 15);
    int site_continent_id = 0;
    err =
        i3d_ping_sites_getter_list_site_continent_id(sites_getter, 0, &site_continent_id);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(site_continent_id == 5);
    char country[64];
    err = i3d_ping_sites_getter_list_site_country(sites_getter, 0, country);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(std::string(country) == "Netherlands");
    int dc_location_id = 0;
    err =
        i3d_ping_sites_getter_list_site_dc_location_id(sites_getter, 0, &dc_location_id);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(dc_location_id == 6);
    char dc_location_name[64];
    err = i3d_ping_sites_getter_list_site_dc_location_name(sites_getter, 0,
                                                           dc_location_name);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(std::string(dc_location_name) == "i3d-eu-west-1");
    char hostname[64];
    err = i3d_ping_sites_getter_list_site_hostname(sites_getter, 0, hostname);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(std::string(hostname) == "nlrtm1-pingbeacon1.sys.i3d.network");
    unsigned int ipv4_size = 0;
    err = i3d_ping_sites_getter_list_site_ipv4_size(sites_getter, 0, &ipv4_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(ipv4_size == 1);
    char ip[16];
    err = i3d_ping_sites_getter_list_site_ipv4_ip(sites_getter, 0, 0, ip);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(std::string(ip) == "213.163.66.59");
    unsigned int ipv6_size = 0;
    err = i3d_ping_sites_getter_list_site_ipv6_size(sites_getter, 0, &ipv6_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(ipv6_size == 0);

    i3d_ping_sites_getter_destroy(sites_getter);
}

TEST_CASE("ping sites using json callback to test payload size bigger than c_api buffers",
          "[sites_getter]") {
    I3dSitesGetterPtr sites_getter = nullptr;
    I3dPingError err = I3D_PING_ERROR_NONE;

    i3d_ping_sites_getter_create(&sites_getter);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    err =
        i3d_ping_sites_getter_set_http_get_callback(sites_getter, test_callback, nullptr);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = i3d_ping_sites_getter_init(sites_getter);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = i3d_ping_sites_getter_update(sites_getter);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    I3dSitesGetterStatus status;
    err = i3d_ping_sites_getter_status(sites_getter, &status);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(status == I3D_SITES_GETTER_STATUS_READY);
    unsigned int size = 0;
    err = i3d_ping_sites_getter_site_list_size(sites_getter, &size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(size == 1);
    int site_continent_id = 0;
    err =
        i3d_ping_sites_getter_list_site_continent_id(sites_getter, 0, &site_continent_id);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(site_continent_id == 5);
    char country[64];
    err = i3d_ping_sites_getter_list_site_country(sites_getter, 0, country);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_BUFFER_IS_TOO_SMALL);
    int dc_location_id = 0;
    err =
        i3d_ping_sites_getter_list_site_dc_location_id(sites_getter, 0, &dc_location_id);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(dc_location_id == 6);
    char dc_location_name[64];
    err = i3d_ping_sites_getter_list_site_dc_location_name(sites_getter, 0,
                                                           dc_location_name);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_BUFFER_IS_TOO_SMALL);
    char hostname[64];
    err = i3d_ping_sites_getter_list_site_hostname(sites_getter, 0, hostname);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_BUFFER_IS_TOO_SMALL);
    unsigned int ipv4_size = 0;
    err = i3d_ping_sites_getter_list_site_ipv4_size(sites_getter, 0, &ipv4_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(ipv4_size == 1);
    char ipv4[16];
    err = i3d_ping_sites_getter_list_site_ipv4_ip(sites_getter, 0, 0, ipv4);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_BUFFER_IS_TOO_SMALL);
    unsigned int ipv6_size = 0;
    err = i3d_ping_sites_getter_list_site_ipv6_size(sites_getter, 0, &ipv6_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(ipv6_size == 1);
    char ipv6[46];
    err = i3d_ping_sites_getter_list_site_ipv4_ip(sites_getter, 0, 0, ipv6);
    REQUIRE(err == I3D_PING_ERROR_VALIDATION_BUFFER_IS_TOO_SMALL);

    i3d_ping_sites_getter_destroy(sites_getter);
}
