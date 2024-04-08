#include <catch.hpp>

#include <curl/curl.h>

#include <one/ping/c_api.h>
#include <one/ping/error.h>

#include <chrono>
#include <thread>
#include <iostream>

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

TEST_CASE("pingers pinging sites", "[pingers]") {
    std::this_thread::sleep_for(std::chrono::seconds(2));

    I3dSitesGetterPtr sites_getter = nullptr;
    I3dIpListPtr ip_list = nullptr;
    I3dPingError err = I3D_PING_ERROR_NONE;

    err = i3d_ping_ip_list_create(&ip_list);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    i3d_ping_sites_getter_create(&sites_getter, http_callback, nullptr);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = i3d_ping_sites_getter_update(sites_getter);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    I3dSitesGetterStatus status;
    err = i3d_ping_sites_getter_status(sites_getter, &status);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(status == I3D_SITES_GETTER_STATUS_READY);
    unsigned int amount_of_ping_sites = 0;
    err = i3d_ping_sites_getter_site_list_size(sites_getter, &amount_of_ping_sites);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    // The number of sites can change over time, so just make sure at least something was
    // pinged.
    REQUIRE(amount_of_ping_sites > 0);

    for (int i=0; i< amount_of_ping_sites; i++) {
        char ip[16];
        err = i3d_ping_sites_getter_list_site_ipv4_ip(sites_getter, i, 0, ip);
        REQUIRE(err == I3D_PING_ERROR_NONE);

        err = i3d_ping_ip_list_push_back(ip_list, ip);
        REQUIRE(err == I3D_PING_ERROR_NONE);
    }
    // err = i3d_ping_ip_list_push_back(ip_list, "162.244.52.60");
    // REQUIRE(err == I3D_PING_ERROR_NONE);
    // err = i3d_ping_ip_list_push_back(ip_list, "109.200.208.52");
    // REQUIRE(err == I3D_PING_ERROR_NONE);
    // err = i3d_ping_ip_list_push_back(ip_list, "138.128.136.54");
    // REQUIRE(err == I3D_PING_ERROR_NONE);
    // unsigned int size = 0;
    // err = i3d_ping_ip_list_size(ip_list, &size);
    // REQUIRE(err == I3D_PING_ERROR_NONE);
    // REQUIRE(size == 4);

    I3dPingersPtr pingers = nullptr;
    err = i3d_ping_pingers_create(&pingers, ip_list);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    std::vector<int> last(amount_of_ping_sites);
    std::vector<double> average(amount_of_ping_sites);
    std::vector<int> min_time(amount_of_ping_sites);
    std::vector<int> max_time(amount_of_ping_sites);
    std::vector<double> median(amount_of_ping_sites);
    std::vector<unsigned int> ping_response_count(amount_of_ping_sites);
    bool result = false;
    bool all_pigned_at_least_once = false;

    for (auto i = 0; i < 100; ++i) {
        err = i3d_ping_pingers_update(pingers);
        REQUIRE(err == I3D_PING_ERROR_NONE);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        for (auto i = 0; i < amount_of_ping_sites; ++i) {
            err = i3d_ping_pingers_statistics(pingers, i, &(last[i]), &(average[i]),
                                              &(min_time[i]), &(max_time[i]),
                                              &(median[i]), &(ping_response_count[i]));

            if (err == I3D_PING_ERROR_NONE) {
                err =
                    i3d_ping_pingers_at_least_one_site_has_been_pinged(pingers, &result);
                REQUIRE(err == I3D_PING_ERROR_NONE);
                REQUIRE(result == true);
            }
        }

        if (!all_pigned_at_least_once) {
            err = i3d_ping_pingers_all_sites_have_been_pinged(pingers,
                                                              &all_pigned_at_least_once);
            REQUIRE(err == I3D_PING_ERROR_NONE);
        }
    }

    for (auto i = 0; i < amount_of_ping_sites; ++i) {
        err = i3d_ping_pingers_statistics(pingers, i, &(last[i]), &(average[i]),
                                          &(min_time[i]), &(max_time[i]), &(median[i]),
                                          &(ping_response_count[i]));
        if (err != I3D_PING_ERROR_NONE) {
            char ip[256];
            err = i3d_ping_ip_list_ip(ip_list, i, ip, sizeof(ip));
            if (err == I3D_PING_ERROR_NONE) {
                std::cout << "failed to ping site with IP(" << ip << ")" << std::endl;
            }
        }
    }

    REQUIRE(all_pigned_at_least_once == true);

    for (auto i = 0; i < amount_of_ping_sites; ++i) {
        err = i3d_ping_pingers_statistics(pingers, i, &(last[i]), &(average[i]),
                                          &(min_time[i]), &(max_time[i]), &(median[i]),
                                          &(ping_response_count[i]));
        REQUIRE(err == I3D_PING_ERROR_NONE);
        REQUIRE(0 < ping_response_count[i]);
    }

    i3d_ping_ip_list_destroy(ip_list);
    i3d_ping_pingers_destroy(pingers);
}


TEST_CASE("ping sites using json callback to test payload size bigger than c_api buffers",
          "[sites_getter]") {
    I3dSitesGetterPtr sites_getter = nullptr;
    I3dPingError err = I3D_PING_ERROR_NONE;

    i3d_ping_sites_getter_create(&sites_getter, test_callback, nullptr);
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