#include <catch.hpp>
#include <util.h>

#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/opcodes.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/internal/version.h>
#include <one/arcus/message.h>

#include <functional>
#include <iostream>

using namespace one;

TEST_CASE("current arcus version", "[arcus]") {
    REQUIRE(arcus_protocol::current_version() == ArcusVersion::V2);
}

TEST_CASE("opcode version V1 validation", "[arcus]") {
    REQUIRE(is_opcode_supported_v2(Opcodes::hello));
    REQUIRE(is_opcode_supported_v2(Opcodes::soft_stop));
    REQUIRE(is_opcode_supported_v2(Opcodes::allocated));
    REQUIRE(is_opcode_supported_v2(Opcodes::live_state_request));
    REQUIRE(is_opcode_supported_v2(Opcodes::live_state));
    REQUIRE(is_opcode_supported_v2(Opcodes::invalid) == false);
}

TEST_CASE("opcode current version validation", "[arcus]") {
    REQUIRE(is_opcode_supported(Opcodes::hello));
    REQUIRE(is_opcode_supported(Opcodes::soft_stop));
    REQUIRE(is_opcode_supported(Opcodes::allocated));
    REQUIRE(is_opcode_supported(Opcodes::live_state_request));
    REQUIRE(is_opcode_supported(Opcodes::live_state));
    REQUIRE(is_opcode_supported(Opcodes::invalid) == false);
}

//------------------------------------------------------------------------------
// Message tests.

void soft_stop_callback(int timeout) {
    std::cout << "timeout is: " << timeout << std::endl;
}

TEST_CASE("soft stop message callback", "[arcus]") {
    Message m;
    m.init(Opcodes::soft_stop, nullptr, 0);
    // REQUIRE(Message::update(m, soft_stop_callback) == 0);
}

//------------------------------------------------------------------------------
// Socket & Connection tests.

void wait_ready(Socket &socket) {
    REQUIRE(socket.select(0.1f) >= 0);
};

void listen(Socket &server, unsigned int &port) {
    REQUIRE(server.init() == 0);
    REQUIRE(server.bind(0) == 0);

    std::string server_ip;
    REQUIRE(server.address(server_ip, port) == 0);
    REQUIRE(server_ip.length() > 0);
    REQUIRE(port != 0);

    REQUIRE(server.set_non_blocking(true) == 0);
    REQUIRE(server.listen(1) == 0);
}

void connect(Socket &client, unsigned int port) {
    client.init();
    REQUIRE(client.connect("127.0.0.1", port) == 0);
    REQUIRE(client.set_non_blocking(true) == 0);
}

void accept(Socket &server, Socket &in_client) {
    // Accept client on server.
    wait_ready(server);
    std::string client_ip;
    unsigned int client_port;
    REQUIRE(server.accept(in_client, client_ip, client_port) == 0);
    REQUIRE(client_ip.length() > 0);
    REQUIRE(client_port != 0);
    REQUIRE(in_client.set_non_blocking(true) == 0);
}

// Socket and Connection normal external behavior.
TEST_CASE("connection", "[arcus]") {
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

    unsigned int server_port;
    listen(server, server_port);

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
    connect(out_client, server_port);

    // Accept client on server.
    accept(server, in_client);

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

    //-------------
    // Handshaking.

    // Arcus connections around the server-side and client-side sockets that
    // are connected to each other.
    auto server_connection = Connection(in_client, 2, 2);
    auto client_connection = Connection(out_client, 2, 2);

    server_connection.initiate_handshake();
    for (auto i = 0; i < 10; ++i) {
        result = server_connection.update();
        REQUIRE(result == 0);
        result = client_connection.update();
        REQUIRE(result == 0);
        sleep(10);
        if (server_connection.status() == Connection::Status::ready &&
            client_connection.status() == Connection::Status::ready)
            break;
    }
    REQUIRE(server_connection.status() == Connection::Status::ready);
    REQUIRE(client_connection.status() == Connection::Status::ready);

    server.close();
    out_client.close();
    in_client.close();
    shutdown_socket_system();
}

// Handshake abnormalities.
TEST_CASE("handshake", "[arcus]") {
    init_socket_system();

    Socket server;
    unsigned int server_port;
    listen(server, server_port);

    Socket out_client;
    connect(out_client, server_port);

    // Accept client on server.
    Socket in_client;
    accept(server, in_client);

    auto server_connection = Connection(in_client, 2, 2);
    server_connection.initiate_handshake();

    //----------------------------------------------------------
    // Test - client sends a hello to server before server does.

    wait_ready(out_client);
    codec::Hello hello = codec::valid_hello();
    auto result = out_client.send(&hello, codec::hello_size());
    REQUIRE(result == codec::hello_size());
    auto did_fail = false;
    for (int i = 0; i < 5; i++) {
        did_fail = server_connection.update() < 0;  // Todo Error type.
        if (did_fail) break;
        sleep(10);
    }
    REQUIRE(did_fail);
    // todo get error reason
    server_connection.reset();

    //----------------------------------
    // Test - client sends bad response.

    // Receive the hello from server.

    // Send wrong thing back.

    // Ensure the connection and socket were closed by server.

    //--------------------------
    // Test - handshake timeout.

    server.close();
    out_client.close();
    in_client.close();
    shutdown_socket_system();
}
