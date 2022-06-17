#include <catch.hpp>

#include <curl/curl.h>

#include <one/ping/c_api.h>
#include <one/ping/error.h>

#include <chrono>
#include <thread>
#include <iostream>

TEST_CASE("pingers pinging sites", "[pingers]") {
    std::this_thread::sleep_for(std::chrono::seconds(2));

    I3dIpListPtr ip_list = nullptr;
    I3dPingError err = I3D_PING_ERROR_NONE;

    err = i3d_ping_ip_list_create(&ip_list);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    err = i3d_ping_ip_list_push_back(ip_list, "213.163.66.59");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = i3d_ping_ip_list_push_back(ip_list, "43.239.136.59");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = i3d_ping_ip_list_push_back(ip_list, "162.244.52.59");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = i3d_ping_ip_list_push_back(ip_list, "185.50.104.59");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    unsigned int size = 0;
    err = i3d_ping_ip_list_size(ip_list, &size);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(size == 4);

    I3dPingersPtr pingers = nullptr;
    err = i3d_ping_pingers_create(&pingers, ip_list);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    std::vector<int> last(size);
    std::vector<double> average(size);
    std::vector<int> min_time(size);
    std::vector<int> max_time(size);
    std::vector<double> median(size);
    std::vector<unsigned int> ping_response_count(size);
    bool result = false;
    bool all_pigned_at_least_once = false;

    for (auto i = 0; i < 100; ++i) {
        err = i3d_ping_pingers_update(pingers);
        REQUIRE(err == I3D_PING_ERROR_NONE);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        for (auto i = 0; i < size; ++i) {
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

    for (auto i = 0; i < size; ++i) {
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

    for (auto i = 0; i < size; ++i) {
        err = i3d_ping_pingers_statistics(pingers, i, &(last[i]), &(average[i]),
                                          &(min_time[i]), &(max_time[i]), &(median[i]),
                                          &(ping_response_count[i]));
        REQUIRE(err == I3D_PING_ERROR_NONE);
        REQUIRE(0 < ping_response_count[i]);
    }

    i3d_ping_ip_list_destroy(ip_list);
    i3d_ping_pingers_destroy(pingers);
}
