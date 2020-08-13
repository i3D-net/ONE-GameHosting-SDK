#include <catch.hpp>
#include <util.h>

#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/opcodes.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/internal/version.h>
#include <one/arcus/error.h>
#include <one/arcus/message.h>

#include <functional>
#include <string>
#include <utility>

using namespace one;

TEST_CASE("current arcus version", "[arcus]") {
    REQUIRE(arcus_protocol::current_version() == ArcusVersion::V2);
}

TEST_CASE("opcode version V2 validation", "[arcus]") {
    REQUIRE(is_opcode_supported_v2(Opcodes::hello));
    REQUIRE(is_opcode_supported_v2(Opcodes::error_response));
    REQUIRE(is_opcode_supported_v2(Opcodes::soft_stop_request));
    REQUIRE(is_opcode_supported_v2(Opcodes::allocated_request));
    REQUIRE(is_opcode_supported_v2(Opcodes::meta_data_request));
    REQUIRE(is_opcode_supported_v2(Opcodes::live_state_request));
    REQUIRE(is_opcode_supported_v2(Opcodes::live_state_response));
    REQUIRE(is_opcode_supported_v2(Opcodes::host_information_request));
    REQUIRE(is_opcode_supported_v2(Opcodes::invalid) == false);
}

TEST_CASE("opcode current version validation", "[arcus]") {
    REQUIRE(is_opcode_supported(Opcodes::hello));
    REQUIRE(is_opcode_supported(Opcodes::error_response));
    REQUIRE(is_opcode_supported(Opcodes::soft_stop_request));
    REQUIRE(is_opcode_supported(Opcodes::allocated_request));
    REQUIRE(is_opcode_supported(Opcodes::meta_data_request));
    REQUIRE(is_opcode_supported(Opcodes::live_state_request));
    REQUIRE(is_opcode_supported(Opcodes::live_state_response));
    REQUIRE(is_opcode_supported(Opcodes::host_information_request));
    REQUIRE(is_opcode_supported(Opcodes::invalid) == false);
}

//------------------------------------------------------------------------------
// Message tests.

TEST_CASE("message handling", "[arcus]") {
    Message m;
    const std::string payload = "{}";
    REQUIRE(m.init(Opcodes::soft_stop_request, {payload.c_str(), payload.size()}) == 0);
    REQUIRE(m.code() == Opcodes::soft_stop_request);
    // FIXME: uncomment when the payload class is properlly implemented.
    // REQUIRE(m.payload().is_empty() == false);
    REQUIRE(m.init(Opcodes::soft_stop_request, {nullptr, 0}) == 0);
    m.reset();
    REQUIRE(m.code() == Opcodes::invalid);
    REQUIRE(m.payload().is_empty());
    REQUIRE(m.init(static_cast<int>(Opcodes::soft_stop_request), {nullptr, 0}) == 0);
    REQUIRE(m.code() == Opcodes::soft_stop_request);
    REQUIRE(m.payload().is_empty());
    m.reset();
    REQUIRE(m.code() == Opcodes::invalid);
    REQUIRE(m.payload().is_empty());
}

TEST_CASE("message prepare", "[arcus]") {
    Message m;
    REQUIRE(messages::prepare_error_response(m) == 0);
    REQUIRE(m.code() == Opcodes::error_response);
    REQUIRE(m.payload().is_empty() == true);

    m.reset();
    REQUIRE(messages::prepare_soft_stop_request(1000, m) == 0);
    REQUIRE(m.code() == Opcodes::soft_stop_request);
    // FIXME: uncomment when the payload class is properlly implemented.
    // REQUIRE(m.payload().is_empty() == false);

    m.reset();
    Array allocated;
    REQUIRE(messages::prepare_allocated_request(allocated, m) == 0);
    REQUIRE(m.code() == Opcodes::allocated_request);
    // FIXME: uncomment when the payload class is properlly implemented.
    // REQUIRE(m.payload().is_empty() == false);

    m.reset();
    Array meta_data;
    REQUIRE(messages::prepare_meta_data_request(meta_data, m) == 0);
    REQUIRE(m.code() == Opcodes::meta_data_request);
    // FIXME: uncomment when the payload class is properlly implemented.
    // REQUIRE(m.payload().is_empty() == false);

    m.reset();
    REQUIRE(messages::prepare_live_state_request(m) == 0);
    REQUIRE(m.code() == Opcodes::live_state_request);
    REQUIRE(m.payload().is_empty() == true);

    m.reset();
    REQUIRE(messages::prepare_live_state_response(1, 16, "test server", "test map", "test mode",
                                                  "test version", m) == 0);
    REQUIRE(m.code() == Opcodes::live_state_response);
    // FIXME: uncomment when the payload class is properlly implemented.
    // REQUIRE(m.payload().is_empty() == false);
}

//------------------------------------------------------------------------------
// Socket & Connection tests.

void wait_ready(Socket &socket) {
    REQUIRE(socket.select(0.1f) >= 0);
};

void listen(Socket &server, unsigned int &port) {
    REQUIRE(!is_error(server.init()));
    REQUIRE(!is_error(server.bind(0)));

    std::string server_ip;
    REQUIRE(!is_error(server.address(server_ip, port)));
    REQUIRE(server_ip.length() > 0);
    REQUIRE(port != 0);

    REQUIRE(!is_error(server.listen(1)));
}

void connect(Socket &client, unsigned int port) {
    client.init();
    REQUIRE(!is_error(client.connect("127.0.0.1", port)));
}

void accept(Socket &server, Socket &in_client) {
    // Accept client on server.
    wait_ready(server);
    std::string client_ip;
    unsigned int client_port;
    REQUIRE(!is_error(server.accept(in_client, client_ip, client_port)));
    REQUIRE(client_ip.length() > 0);
    REQUIRE(client_port != 0);
}

// Socket and Connection normal external behavior.
TEST_CASE("connection", "[arcus]") {
    init_socket_system();
    Socket server;

    //-----------
    // Lifecycle.
    REQUIRE(server.is_initialized() == false);
    REQUIRE(!is_error(server.init()));
    REQUIRE(server.is_initialized() == true);
    REQUIRE(!is_error(server.close()));
    REQUIRE(server.is_initialized() == false);

    //---------------
    // Server listen.

    unsigned int server_port;
    listen(server, server_port);

    // Confirm no incoming connections.
    Socket in_client;
    std::string client_ip;
    unsigned int client_port;
    REQUIRE(!is_error(server.accept(in_client, client_ip, client_port)));
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
    size_t sent;
    auto result = out_client.send(&out_data, 1, sent);
    REQUIRE(!is_error(result));
    REQUIRE(sent == 1);

    // Receive on accepted server-side client socket.
    wait_ready(in_client);
    unsigned char in_data[128] = {0};
    size_t received;
    result = in_client.receive(in_data, 128, received);
    REQUIRE(!is_error(result));
    REQUIRE(received == 1);
    REQUIRE(in_data[0] == out_data);

    //-------------
    // Handshaking.

    // Arcus connections around the server-side and client-side sockets that
    // are connected to each other.
    auto server_connection = Connection(in_client, 2, 2);
    auto client_connection = Connection(out_client, 2, 2);

    server_connection.initiate_handshake();
    for (auto i = 0; i < 10; ++i) {
        REQUIRE(!is_error(server_connection.update()));
        REQUIRE(!is_error(client_connection.update()));
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

struct HandshakeTestObjects {
    Socket server;
    Socket out_client;
    Socket in_client;
    Connection *server_connection;
    unsigned int server_port;
};

void init_handshake_test(HandshakeTestObjects &objects) {
    init_socket_system();

    listen(objects.server, objects.server_port);
    connect(objects.out_client, objects.server_port);
    accept(objects.server, objects.in_client);
    objects.server_connection = new Connection(objects.in_client, 2, 2);
}

void shutdown_handshake_test(HandshakeTestObjects &objects) {
    objects.server.close();
    objects.out_client.close();
    objects.in_client.close();
    delete objects.server_connection;
    shutdown_socket_system();
}

TEST_CASE("handshake early hello", "[arcus]") {
    HandshakeTestObjects objects;
    init_handshake_test(objects);

    objects.server_connection->initiate_handshake();
    wait_ready(objects.out_client);
    codec::Hello hello = codec::valid_hello();
    size_t sent;
    auto result = objects.out_client.send(&hello, codec::hello_size(), sent);
    REQUIRE(!is_error(result));
    REQUIRE(sent == codec::hello_size());
    auto did_fail = false;
    for (int i = 0; i < 5; i++) {
        did_fail = is_error(objects.server_connection->update());  // Todo Error type.
        if (did_fail) break;
        sleep(10);
    }
    REQUIRE(did_fail);
    // todo get error reason

    shutdown_handshake_test(objects);
}

TEST_CASE("handshake hello bad response", "[arcus]") {
    HandshakeTestObjects objects;
    init_handshake_test(objects);

    // Wait for server to enter sent state.
    objects.server_connection->initiate_handshake();
    for (int i = 0; i < 10; i++) {
        REQUIRE(!is_error(objects.server_connection->update()));
        if (objects.server_connection->status() == Connection::Status::handshake_hello_sent) {
            break;
        }
        sleep(1);
    }
    REQUIRE(objects.server_connection->status() == Connection::Status::handshake_hello_sent);

    // Receive the hello from server.
    codec::Hello hello = {0};
    size_t received;
    auto result = objects.out_client.receive(&hello, codec::hello_size(), received);
    REQUIRE(!is_error(result));
    REQUIRE(received == codec::hello_size());
    REQUIRE(codec::validate_hello(hello));

    // Send wrong opcode back.
    static codec::Header hello_header = {0};
    hello_header.opcode = static_cast<char>(Opcodes::soft_stop_request);
    size_t sent;
    result = objects.out_client.send(&hello_header, codec::header_size(), sent);
    REQUIRE(!is_error(result));
    REQUIRE(sent == codec::header_size());

    // Ensure the server connection enters an error state.
    Error err;
    for (int i = 0; i < 10; i++) {
        err = (objects.server_connection->update());
        if (is_error(err)) {
            break;
        }
        sleep(1);
    }
    REQUIRE(err == ONE_ERROR_CONNECTION_HELLO_MESSAGE_REPLY_INVALID);

    shutdown_handshake_test(objects);
}

//--------------------------
// Test - handshake timeout.
TEST_CASE("handshake timeout", "[arcus]") {}
