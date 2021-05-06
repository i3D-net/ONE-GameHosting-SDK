#include <catch.hpp>

#include <one/ping/internal/pinger.h>

#include <chrono>
#include <thread>

using namespace i3d::ping;

TEST_CASE("pinger statistics", "[pinger]") {
    init_socket_system();

    Pinger pinger;

    int last = 0;
    double average = 0.0;

    auto err = pinger.last_time(last);
    REQUIRE(err == I3D_PING_ERROR_PINGER_IS_UNINITIALIZED);

    err = pinger.average_time(average);
    REQUIRE(err == I3D_PING_ERROR_PINGER_IS_UNINITIALIZED);

    err = pinger.init("8.8.8.8");
    REQUIRE(err == I3D_PING_ERROR_NONE);

    err = pinger.last_time(last);
    REQUIRE(err == I3D_PING_ERROR_PINGER_INVALID_TIME);
    err = pinger.average_time(average);
    REQUIRE(err == I3D_PING_ERROR_PINGER_INVALID_TIME);

    for (auto i = 0; i < 100; ++i) {
        err = pinger.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(err == I3D_PING_ERROR_NONE);

        err = pinger.last_time(last);
        if (err == I3D_PING_ERROR_NONE) {
            break;
        }
    }

    err = pinger.last_time(last);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = pinger.average_time(average);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    REQUIRE(last == average);

    for (auto i = 0; i < 100; ++i) {
        err = pinger.update();
        REQUIRE(err == I3D_PING_ERROR_NONE);

        err = pinger.last_time(last);
        if (err == I3D_PING_ERROR_NONE) {
            break;
        }
    }

    err = pinger.last_time(last);
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = pinger.average_time(average);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    shutdown_socket_system();
}
