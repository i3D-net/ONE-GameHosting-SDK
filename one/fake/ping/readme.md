# Fake Game showing use of Ping component

This folder contains a fake game that uses the ping component. The code here can be used both as a reference for how real game servers can integrate and use the library, or as a way to test the library for correctness.

## Dependencies

A game integration requires the ping library located in one/ping.

## Overview

The i3d_ip_list_wrapper.h/cpp files provide utlities for working with lists returned by the API.

The i3d_sites_getter_wrapper.h/cpp files provide access to a list of available servers on the i3D platform for games to connect to.

The i3d_pingers_wrapper.h/cpp files allow for ping latency measurements to a given list of servers.

The integration is intended to be as simple as possible, but please let us know if you have any suggestions for improvement.

The main points of an integration are summarized below, however you may refer to the above files for complete working examples, including optional features such as logging and allocation hooks.

### 1. Get the list of servers on the i3D ONE Platform

The i3d_sites_getter_wrapper.cpp file handles all of the following however they are described here to illustrate the main usage pattern of the c_api.h.

Include headers
```c++
#include <one/ping/c_api.h>
#include <one/ping/c_error.h>
```

Create the Sites Getter.
```c++
I3dSitesGetterPtr sites_getter;
I3dPingError err = i3d_ping_sites_getter_create(&sites_getter, callback, userdata);
if (i3d_ping_is_error(err)) {
    // Handle error.
    // A text representation of the error can be obtained via:
    // i3d_ping_error_text(err)), for easier debugging or logging.
}
```

Update it to fetch the site information.
```c++
I3dPingError err = i3d_ping_sites_getter_update(sites_getter);
if (i3d_ping_is_error(err)) {
    // Handle error.
}
```

Then the site information can be passed to the pinger, as shown in the following section. Make sure to destroy the sites getters to avoid a leak:
```c++
i3d_sites_getter_destroy(sites_getter);
```

### 2. Ping a list of servers to obtain latency readings

The i3d_pingers_wrapper.cpp file handles all of the following however they are described here to illustrate the main usage pattern of the c_api.h.

Include headers
```c++
#include <one/ping/c_api.h>
#include <one/ping/c_error.h>
```

Get the server sites list to send to the pinger. This is a continuation of the previous section that shows how to use the sites getter API.
```c++
I3dIpListPtr ip_list
I3dPingError err = i3d_ping_ip_list_create(&ip_list);
if (i3d_ping_is_error(err)) {
    // Handle error.
}
err = i3d_ping_sites_getter_ipv4_list(sites_getter, ip_list);
if (i3d_ping_is_error(err)) {
    // Handle error.
}
```

Create the Pingers, passing it the ip_list from the previous step.
```c++
I3dPingersPtr pingers;
I3dPingError err = i3d_ping_pingers_create(&pingers, ip_list);
if (i3d_ping_is_error(err)) {
    // Handle error.
}
```

Update the pingers to process tcp pings and accumulate results.
```c++
    I3dPingError err = i3d_ping_pingers_update(_pingers);
    if (i3d_ping_is_error(err)) {
        // Handle error.
    }
```

Statistics for each site can be queried by iterating. There are also utility functions `i3d_ping_pingers_at_least_one_site_has_been_pinged` and `all_sites_have_been_pinged` to check if data is available.
```c++
// Local storage struct for the results..
struct PingStatistics {
    PingStatistics()
        : last_time(0)
        , average_time(0.0)
        , min_time(0)
        , max_time(0.0)
        , median_time(0)
        , ping_response_count(0) {}

    int last_time;
    double average_time;
    int min_time;
    int max_time;
    double median_time;
    unsigned int ping_response_count;
} statistics;

// Iterate each site request.
const int num_sites = i3d_ping_ip_list_size(ip_list);
for (i := 0; i < num_sites, ++i) {
    I3dPingError err = i3d_ping_pingers_statistics(
        pingers, i, &(statistics.last_time), &(statistics.average_time),
        &(statistics.min_time), &(statistics.max_time), &(statistics.median_time),
        &(statistics.ping_response_count));
    if (i3d_ping_is_error(err)) {
        // Handle error.
    }
    if (statistics_last_time < 0) {
        // No ping results for this site yet...
        continue;
    }
    //...
}
```

Make sure to destroy the ip_list and pingers when finished.
```c++
i3d_ping_ip_list_destroy(ip_list);
i3d_ping_pingers_destroy(pingers);
```