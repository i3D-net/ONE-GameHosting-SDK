#include <catch.hpp>
#include <one/arcus/c_error.h>
#include <one/arcus/c_api.h>

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
    one_server_create(&server);
    one_server_set_logger(server, log, &_userdata);
    // Logger should print debug message during destruction.
    one_server_destroy(server);
    REQUIRE(_was_log_called);
}