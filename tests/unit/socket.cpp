#include <string>

#include "../catch2/catch.hpp"
#include "../../one/arcus/internal/socket.h"

using namespace one;

TEST_CASE("socket", "[arcus]") {
    init_socket_system();
    Socket server;

    // Lifecycle.
    REQUIRE(server.is_initialized() == false);
    REQUIRE(server.init() == 0);
    REQUIRE(server.is_initialized() == true);
    REQUIRE(server.close() == 0);
    REQUIRE(server.is_initialized() == false);

    // Server.
    REQUIRE(server.init() == 0);
    REQUIRE(server.bind(0) == 0);
    REQUIRE(server.listen(1) == 0);
    Socket client;
    std::string ip;
    unsigned int port;
    REQUIRE(server.set_non_blocking(true) == 0);
    REQUIRE(server.accept(client, ip, port) == 0);
}