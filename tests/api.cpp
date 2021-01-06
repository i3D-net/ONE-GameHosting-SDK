#include <catch.hpp>
#include <one/arcus/c_error.h>
#include <one/arcus/c_api.h>
#include <one/arcus/platform.h>
#include <one/arcus/server.h>
#include <tests/util.h>

// C API tests.
// Most of the api is indirectly tested via the integration tests, however
// more detailed tests may be added here.

char _userdata; // Arbitrary object to pass as userdata.
bool _was_log_called = false;

// Custom logger endpoint.
void log(void *userdata, OneLogLevel level, const char *message) {
    REQUIRE(userdata == (void *)&_userdata);
    REQUIRE(level == ONE_LOG_LEVEL_INFO);
    REQUIRE(message != nullptr);
    _was_log_called = true;
}

TEST_CASE("customer logger", "[capi]") {
    OneServerPtr server;
    one_server_create(9001, &server);
    one_server_set_logger(server, log, &_userdata);
    // Logger should print debug message during destruction.
    one_server_destroy(server);
    REQUIRE(_was_log_called);
}

#ifdef ONE_WINDOWS // On linux, listen may succeed even if already listened on.
TEST_CASE("server port retry", "[capi]") {
    i3d::one::server::set_listen_retry_delay(1);

    constexpr auto port = 9002;

    // Block a port.
    OneServerPtr a;
    auto err = one_server_create(port, &a);
    REQUIRE(!one_is_error(err));

    // Start a new server on same
    OneServerPtr b;
    err = one_server_create(port, &b);
    REQUIRE(!one_is_error(err));

    // Pump updates.
    i3d::one::wait_until(3000, [&]() -> bool {
        // First server should be fine.
        err = one_server_update(a);
        REQUIRE(!one_is_error(err));

        // Second should complain of port issues.
        err = one_server_update(b);
        bool hasListenError = (err == ONE_ERROR_SERVER_RETRYING_LISTEN || err == ONE_ERROR_SOCKET_BIND_FAILED);
        REQUIRE(hasListenError);
        
        return false;
    });

    // Now destroy the first server, which should free the port.
    one_server_destroy(a);

    // Update the second server and it should listen successfully.
    REQUIRE(i3d::one::wait_until(1500, [&]() -> bool {
        // First server should be fine.
        err = one_server_update(b);

        return !one_is_error(err);
    }));
}
#endif