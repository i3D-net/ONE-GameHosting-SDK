#include <string>

#include "../catch2/catch.hpp"
#include "../../one/arcus/internal/socket.h"

using namespace one;

TEST_CASE("socket", "[arcus]") {
    init_socket_system();
    Socket server;

    //-----------
    // Lifecycle.
    REQUIRE(server.is_initialized() == false);
    REQUIRE(server.init() == 0);
    REQUIRE(server.is_initialized() == true);
    REQUIRE(server.close() == 0);
    REQUIRE(server.is_initialized() == false);

    //---------------
    // Server listen.

    // Init socket.
    REQUIRE(server.init() == 0);
    REQUIRE(server.bind(0) == 0);
    std::string server_ip;
    unsigned int server_port;
    REQUIRE(server.address(server_ip, server_port) == 0);
    REQUIRE(server_ip.length() > 0);
    REQUIRE(server_port != 0);
    REQUIRE(server.set_non_blocking(true) == 0);

    // Listen.
    REQUIRE(server.listen(1) == 0);

    // Confirm no incoming connections.
    Socket in_client;
    std::string client_ip;
    unsigned int client_port;
    REQUIRE(server.accept(in_client, client_ip, client_port) == 0);
    REQUIRE(in_client.is_initialized() == false);

    //--------------------------
    // Client connect to server.

    // Init client socket.
    Socket out_client;
    out_client.init();
    REQUIRE(out_client.is_initialized() == true);

    auto wait_ready = [](Socket &socket) {
        bool is_ready;
        REQUIRE(socket.ready(0.05f, is_ready) == 0);
        REQUIRE(is_ready);
    };

    // Connect to server. Wait for connection to establish, with a timeout.
    REQUIRE(out_client.connect("127.0.0.1", server_port) == 0);

    // Accept client on server.
    wait_ready(server);
    REQUIRE(server.accept(in_client, client_ip, client_port) == 0);
    REQUIRE(in_client.is_initialized() == true);
    REQUIRE(client_ip.length() > 0);
    REQUIRE(client_port != 0);

    //------------------
    // Send and receive.

    // Send to server.
    wait_ready(out_client);
    const unsigned char out_data = 'a';
    auto result = out_client.send(&out_data, 1);
    REQUIRE(result == 1);

    // Receive on accepted server-side client socket.
    wait_ready(in_client);
    unsigned char in_data[128] = {0};
    REQUIRE(in_client.receive(in_data, 128) == 1);
    REQUIRE(in_data[0] == out_data);
}