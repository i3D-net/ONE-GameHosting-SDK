#include "../catch2/catch.hpp"
#include "../../one/arcus/internal/socket.h"

using namespace one;

TEST_CASE("socket", "[arcus]") {
    init_socket_system();
    Socket socket;
    REQUIRE(socket.is_initialized() == false);
    REQUIRE(socket.init() == 0);
    REQUIRE(socket.is_initialized() == true);
    REQUIRE(socket.close() == 0);
    REQUIRE(socket.is_initialized() == false);
}