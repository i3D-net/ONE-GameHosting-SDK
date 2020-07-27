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

    // Server listen.
    REQUIRE(server.init() == 0);
    REQUIRE(server.bind(0) == 0);
    std::string server_ip;
    unsigned int server_port;
    REQUIRE(server.address(server_ip, server_port) == 0);
    REQUIRE(server_ip.length() > 0);
    REQUIRE(server_port != 0);
    REQUIRE(server.listen(1) == 0);
    Socket in_client;
    std::string client_ip;
    unsigned int client_port;
    REQUIRE(server.set_non_blocking(true) == 0);
    REQUIRE(server.accept(in_client, client_ip, client_port) == 0);
    REQUIRE(in_client.is_initialized() == false);

    // Client connect to server.
    Socket out_client;
    out_client.init();
    REQUIRE(out_client.connect("127.0.0.1", server_port) == 0);
    REQUIRE(server.accept(in_client, client_ip, client_port) == 0);
    REQUIRE(in_client.is_initialized() == true);
    REQUIRE(client_ip.length() > 0);
    REQUIRE(client_port != 0);
}