// This file contains an example of code needed in a game integration to
// use the ping component.

#include <one/fake/ping/i3d_allocator.h>
#include <one/fake/ping/i3d_ip_list_wrapper.h>
#include <one/fake/ping/i3d_pingers_wrapper.h>
#include <one/fake/ping/i3d_sites_getter_wrapper.h>

#include <one/fake/ping/log.h>

#include <one/ping/c_api.h>

#include <chrono>
#include <string>
#include <thread>

using namespace i3d_ping_integration;

namespace {

// The game integration must supply its own HTTP hooks. For the purpose of this
// example, this HTTP hook simply supplies a hard-coded response instead of
// actually making the HTTP request.
// To see an example with libcurl look at the [ping
// tests](../../../tests/one/ping/sites_getter.cpp).
void http_callback(const char *url,
                   void (*parsing_callback)(bool success, const char *json,
                                            void *parsing_userdata),
                   void *parsing_userdata, void *http_get_metadata) {

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

    // This is the fake HTTP response. A real integration would make an HTTP
    // request instead with the given URL parameters.
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

    // After the HTTP request has been made, the response is given back to the
    // ping component.
    parsing_callback(true, json.c_str(), parsing_userdata);
}

void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

}  // namespace

int main(int argc, char **argv) {
    // Override the allocator functions.
    // This step is optional and provided just for example purposes, as it some
    // games prefer to control all memory allocations.
    i3d_ping_allocator_set_alloc(allocation::alloc);
    i3d_ping_allocator_set_free(allocation::free);
    i3d_ping_allocator_set_realloc(allocation::realloc);

    I3dSitesGetterWrapper sites_getter;
    bool result = sites_getter.init(http_callback, nullptr);
    if (!result) {
        return -1;
    }

    for (int i = 0; i < 10; ++i) {
        result = sites_getter.update(true);
        if (!result) {
            return -1;
        }

        sleep(100);
    }

    I3dSitesGetterWrapper::Status sites_getter_status = sites_getter.status();
    if (sites_getter_status != I3dSitesGetterWrapper::Status::ready) {
        return -1;
    }

    I3dIpListWrapper ip_list;
    result = sites_getter.ipv4_list(ip_list.list());
    if (!result) {
        return -1;
    }

    I3dPingersWrapper pingers;
    result = pingers.init(ip_list.list());
    if (!result) {
        return -1;
    }

    I3dPingersWrapper::Status pinger_status = pingers.status();
    if (pinger_status != I3dPingersWrapper::Status::initialized) {
        return -1;
    }

    // Update the pingers until it has finished. In a real integration this
    // would be incorporated into a game update.
    bool all_pinged = false;
    for (auto i = 0; i < 100; ++i) {
        result = pingers.update(true);
        if (!result) {
            return -1;
        }

        result = pingers.all_sites_have_been_pinged(all_pinged);
        if (!result) {
            return -1;
        }

        if (all_pinged) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    result = pingers.all_sites_have_been_pinged(all_pinged);
    if (!result) {
        return -1;
    }

    if (!all_pinged) {
        return -1;
    }

    unsigned int size = 0;
    result = pingers.size(size);
    if (!result) {
        return -1;
    }

    I3dPingersWrapper::PingStatistics statistics{};

    L_INFO("ping statistics:");

    for (auto i = 0; i < size; ++i) {
        result = pingers.statistics(i, statistics);
        if (!result) {
            return -1;
        }

        std::string hostname;
        result = sites_getter.site_hostname(i, hostname);
        if (!result) {
            return -1;
        }

        L_INFO("position:" + std::to_string(i));
        L_INFO("\thostname: " + hostname);
        L_INFO("\tlast time:" + std::to_string(statistics.last_time));
        L_INFO("\taverage time: " + std::to_string(statistics.average_time));
        L_INFO("\tmin time:" + std::to_string(statistics.min_time));
        L_INFO("\tmax time:" + std::to_string(statistics.max_time));
        L_INFO("\tmedian time:" + std::to_string(statistics.median_time));
        L_INFO("\tping response count:" + std::to_string(statistics.ping_response_count));
    }

    L_INFO("--------");

    return 0;
}
