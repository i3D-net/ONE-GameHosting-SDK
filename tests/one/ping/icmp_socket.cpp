#include <catch.hpp>

#include <one/ping/c_platform.h>
#include <one/ping/internal/icmp_socket.h>
#include <one/ping/c_error.h>

#include <chrono>
#include <thread>

using namespace i3d::ping;

TEST_CASE("icmp header size validation", "[icmp socket]") {
    IPHeader header;
    // Ensuring that the IPHeader padding buffer is at least as big as the ICMPHeader.
    REQUIRE(sizeof(header.buffer) > sizeof(ICMPHeader));
}

TEST_CASE("icmp socket life cycle", "[icmp socket]") {
#ifdef I3D_PING_WINDOWS
    init_socket_system();
    IcmpSocket socket;
    auto err = socket.init("8.8.8.8");
    REQUIRE(err == I3D_PING_ERROR_NONE);

    shutdown_socket_system();
#else
    IcmpSocket socket;
    auto err = socket.init("8.8.8.8");
    REQUIRE(err == I3D_PING_ERROR_NONE);
#endif
}

TEST_CASE("ping host", "[icmp socket]") {
    init_socket_system();

    IcmpSocket socket;
    int time = 0;

    auto err = socket.time_milliseconds(time);
    REQUIRE(err == I3D_PING_ERROR_SOCKET_INVALID_TIME);

    err = socket.init("8.8.8.8");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = socket.time_milliseconds(time);
    REQUIRE(err == I3D_PING_ERROR_SOCKET_INVALID_TIME);

    for (auto i = 0; i < 100; ++i) {
        err = socket.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(err == I3D_PING_ERROR_NONE);

        err = socket.time_milliseconds(time);

        if (err == I3D_PING_ERROR_SOCKET_INVALID_TIME) {
            continue;
        }

        if (err == I3D_PING_ERROR_NONE) {
            break;
        }
    }

    err = socket.time_milliseconds(time);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    shutdown_socket_system();
}

TEST_CASE("icmp reset", "[icmp socket]") {
    init_socket_system();

    IcmpSocket socket;
    int time = 0;

    auto err = socket.time_milliseconds(time);
    REQUIRE(err == I3D_PING_ERROR_SOCKET_INVALID_TIME);

    err = socket.init("8.8.8.8");
    REQUIRE(err == I3D_PING_ERROR_NONE);
    err = socket.time_milliseconds(time);
    REQUIRE(err == I3D_PING_ERROR_SOCKET_INVALID_TIME);

    for (auto i = 0; i < 100; ++i) {
        err = socket.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(err == I3D_PING_ERROR_NONE);

        err = socket.time_milliseconds(time);

        if (err == I3D_PING_ERROR_SOCKET_INVALID_TIME) {
            continue;
        }

        if (err == I3D_PING_ERROR_NONE) {
            break;
        }
    }

    REQUIRE(socket.status() == IcmpSocket::Status::ping_received);
    err = socket.time_milliseconds(time);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    err = socket.update();
    REQUIRE(err == I3D_PING_ERROR_NONE);
    auto s = socket.status();
    REQUIRE(socket.status() == IcmpSocket::Status::initialized);
    err = socket.time_milliseconds(time);

    REQUIRE(err == I3D_PING_ERROR_SOCKET_INVALID_TIME);

    for (auto i = 0; i < 100; ++i) {
        err = socket.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        REQUIRE(err == I3D_PING_ERROR_NONE);
        auto s = socket.status();

        err = socket.time_milliseconds(time);

        if (err == I3D_PING_ERROR_SOCKET_INVALID_TIME) {
            continue;
        }

        if (err == I3D_PING_ERROR_NONE) {
            break;
        }
    }

    err = socket.time_milliseconds(time);
    REQUIRE(err == I3D_PING_ERROR_NONE);

    shutdown_socket_system();
}
