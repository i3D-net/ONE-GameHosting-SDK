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
};
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
    unsigned int country_size = 0;
    err = i3d_ping_sites_getter_list_site_country_size(sites_getter, 0, &country_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(country_size == 11);
    char country[11];
    err = i3d_ping_sites_getter_list_site_country(sites_getter, 0, country, 11);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(std::string(country, 11) == "Netherlands");
    int dc_location_id = 0;
    err =
        i3d_ping_sites_getter_list_site_dc_location_id(sites_getter, 0, &dc_location_id);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(dc_location_id == 6);
    unsigned int dc_location_name_size = 0;
    err = i3d_ping_sites_getter_list_site_dc_location_name_size(sites_getter, 0,
                                                                &dc_location_name_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(dc_location_name_size == 13);
    char dc_location_name[13];
    err = i3d_ping_sites_getter_list_site_dc_location_name(sites_getter, 0,
                                                           dc_location_name, 13);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(std::string(dc_location_name, 13) == "i3d-eu-west-1");
    unsigned int hostname_size = 0;
    err = i3d_ping_sites_getter_list_site_hostname_size(sites_getter, 0, &hostname_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(hostname_size == 34);
    char hostname[34];
    err = i3d_ping_sites_getter_list_site_hostname(sites_getter, 0, hostname, 34);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(std::string(hostname, 34) == "nlrtm1-pingbeacon1.sys.i3d.network");
    unsigned int ipv4_size = 0;
    err = i3d_ping_sites_getter_list_site_ipv4_size(sites_getter, 0, &ipv4_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(ipv4_size == 1);
    unsigned int ip_size = 0;
    err = i3d_ping_sites_getter_list_site_ipv4_ip_size(sites_getter, 0, 0, &ip_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(ip_size == 13);
    char ip[13];
    err = i3d_ping_sites_getter_list_site_ipv4_ip(sites_getter, 0, 0, ip, 13);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(std::string(ip, 13) == "213.163.66.59");
    unsigned int ipv6_size = 0;
    err = i3d_ping_sites_getter_list_site_ipv4_size(sites_getter, 0, &ipv6_size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(ipv6_size == 1);

    i3d_ping_sites_getter_destroy(sites_getter);
}
