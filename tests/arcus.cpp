#include <catch.hpp>
#include <util.h>

#include <one/arcus/array.h>
#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/internal/version.h>
#include <one/arcus/error.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/arcus/opcode.h>

#include <functional>
#include <string>
#include <utility>

using namespace i3d::one;

TEST_CASE("current arcus version", "[arcus]") {
    REQUIRE(arcus_protocol::current_version() == ArcusVersion::V2);
}

TEST_CASE("opcode version V2 validation", "[arcus]") {
    REQUIRE(is_opcode_supported_v2(Opcode::health));
    REQUIRE(is_opcode_supported_v2(Opcode::hello));
    REQUIRE(is_opcode_supported_v2(Opcode::soft_stop));
    REQUIRE(is_opcode_supported_v2(Opcode::allocated));
    REQUIRE(is_opcode_supported_v2(Opcode::metadata));
    REQUIRE(is_opcode_supported_v2(Opcode::host_information));
    REQUIRE(is_opcode_supported_v2(Opcode::application_instance_information));
    REQUIRE(is_opcode_supported_v2(Opcode::application_instance_status));
    REQUIRE(is_opcode_supported_v2(Opcode::invalid) == false);
}

TEST_CASE("opcode current version validation", "[arcus]") {
    REQUIRE(is_opcode_supported(Opcode::health));
    REQUIRE(is_opcode_supported(Opcode::hello));
    REQUIRE(is_opcode_supported(Opcode::soft_stop));
    REQUIRE(is_opcode_supported(Opcode::allocated));
    REQUIRE(is_opcode_supported(Opcode::metadata));
    REQUIRE(is_opcode_supported(Opcode::host_information));
    REQUIRE(is_opcode_supported(Opcode::application_instance_information));
    REQUIRE(is_opcode_supported(Opcode::application_instance_status));
    REQUIRE(is_opcode_supported(Opcode::invalid) == false);
}

//------------------------------------------------------------------------------
// Message tests.

TEST_CASE("message handling", "[arcus]") {
    Message m;
    const std::string payload = "{\"timeout\":1000}";
    REQUIRE(!is_error(m.init(Opcode::soft_stop, {payload.c_str(), payload.size()})));
    REQUIRE(m.code() == Opcode::soft_stop);
    REQUIRE(m.payload().is_empty() == false);
    REQUIRE(is_error(m.init(Opcode::soft_stop, {nullptr, 0})));
    m.reset();
    REQUIRE(m.code() == Opcode::invalid);
    REQUIRE(m.payload().is_empty());
    REQUIRE(!is_error(m.init(Opcode::soft_stop, {"{}", 2})));
    REQUIRE(m.code() == Opcode::soft_stop);
    REQUIRE(m.payload().is_empty());
    m.reset();
    REQUIRE(m.code() == Opcode::invalid);
    REQUIRE(m.payload().is_empty());
}

//------------------------------------------------------------------------------
// Socket and Connection tests.

void wait_ready_for_read(Socket &socket) {
    bool is_ready;
    REQUIRE(!is_error(socket.ready_for_read(0.1f, is_ready)));
    REQUIRE(is_ready);
};

void wait_ready_for_send(Socket &socket) {
    bool is_ready;
    REQUIRE(!is_error(socket.ready_for_send(0.1f, is_ready)));
    REQUIRE(is_ready);
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
    wait_ready_for_read(server);
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
    wait_ready_for_send(out_client);
    const unsigned char out_data = 'a';
    size_t sent = 0;
    auto result = out_client.send(&out_data, 1, sent);
    REQUIRE(!is_error(result));
    REQUIRE(sent == 1);

    // Receive on accepted server-side client socket.
    wait_ready_for_read(in_client);
    unsigned char in_data[128] = {0};
    size_t received = 0;
    result = in_client.receive(in_data, 128, received);
    REQUIRE(!is_error(result));
    REQUIRE(received == 1);
    REQUIRE(in_data[0] == out_data);

    //-------------
    // Handshaking.

    // Arcus connections around the server-side and client-side sockets that
    // are connected to each other.
    Connection server_connection(2, 2);
    server_connection.init(in_client);
    Connection client_connection(2, 2);
    client_connection.init(out_client);

    server_connection.initiate_handshake();
    for (auto i = 0; i < 10; ++i) {
        REQUIRE(server_connection.update() == ONE_ERROR_NONE);
        REQUIRE(client_connection.update() == ONE_ERROR_NONE);
        sleep(10);
        if (server_connection.status() == Connection::Status::ready &&
            client_connection.status() == Connection::Status::ready)
            break;
    }
    REQUIRE(server_connection.status() == Connection::Status::ready);
    REQUIRE(client_connection.status() == Connection::Status::ready);

    // After handshake, the connection is ready to send and receive only
    // Messages. Sending an invalid message should result in a error on server
    // connection.
    wait_ready_for_send(out_client);
    // Send at last the size of a full message header or else the data will
    // just wait in incoming buffer and be be processed.
    const unsigned char bad_data[codec::header_size()] = {0xF};
    auto err = out_client.send(&bad_data, codec::header_size(), sent);
    REQUIRE(!is_error(err));
    REQUIRE(sent == codec::header_size());
    bool received_bad_header_error = false;
    for_sleep(10, 10, [&]() {
        err = server_connection.update();
        // The update may succeed if the message isn't read during this update,
        // or it should fail with the invalid message code.
        received_bad_header_error =
            received_bad_header_error || (err == ONE_ERROR_CODEC_INVALID_HEADER);
        const bool is_expected =
            (err == ONE_ERROR_CODEC_INVALID_HEADER) || (err == ONE_ERROR_NONE);
        REQUIRE(is_expected);
        if (server_connection.status() == Connection::Status::error) return true;
        return false;
    });
    REQUIRE(received_bad_header_error);
    REQUIRE(server_connection.status() == Connection::Status::error);

    server.close();
    out_client.close();
    in_client.close();
    shutdown_socket_system();
}

struct ClientServerTestObjects {
    Socket server;
    Socket out_client;
    Socket in_client;
    Connection *server_connection;
    Connection *client_connection;
    unsigned int server_port;
};

void init_client_server_test(ClientServerTestObjects &objects,
                             size_t message_queue_length) {
    init_socket_system();

    listen(objects.server, objects.server_port);
    connect(objects.out_client, objects.server_port);
    accept(objects.server, objects.in_client);
    objects.server_connection =
        new Connection(message_queue_length, message_queue_length);
    objects.server_connection->init(objects.in_client);
    objects.client_connection =
        new Connection(message_queue_length, message_queue_length);
    objects.client_connection->init(objects.out_client);
}

void shutdown_client_server_test(ClientServerTestObjects &objects) {
    delete objects.server_connection;
    delete objects.client_connection;
    objects.server.close();
    objects.out_client.close();
    objects.in_client.close();
    shutdown_socket_system();
}

void handshake_client_server_test(ClientServerTestObjects &objects) {
    objects.server_connection->initiate_handshake();
    for_sleep(10, 1, [&]() {
        REQUIRE(!is_error(objects.server_connection->update()));
        REQUIRE(!is_error(objects.client_connection->update()));
        if (objects.server_connection->status() == Connection::Status::ready &&
            objects.client_connection->status() == Connection::Status::ready)
            return true;
        return false;
    });
    REQUIRE(objects.server_connection->status() == Connection::Status::ready);
    REQUIRE(objects.client_connection->status() == Connection::Status::ready);
}

TEST_CASE("handshake early hello", "[arcus]") {
    ClientServerTestObjects objects;
    init_client_server_test(objects, 2);

    objects.server_connection->initiate_handshake();
    wait_ready_for_send(objects.out_client);
    codec::Hello hello = codec::valid_hello();
    size_t sent = 0;
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

    shutdown_client_server_test(objects);
}

TEST_CASE("handshake hello bad response", "[arcus]") {
    ClientServerTestObjects objects;
    init_client_server_test(objects, 2);

    // Wait for server to enter sent state.
    objects.server_connection->initiate_handshake();
    for (int i = 0; i < 10; i++) {
        REQUIRE(!is_error(objects.server_connection->update()));
        if (objects.server_connection->status() ==
            Connection::Status::handshake_hello_sent) {
            break;
        }
        sleep(1);
    }
    REQUIRE(objects.server_connection->status() ==
            Connection::Status::handshake_hello_sent);

    // Receive the hello from server.
    codec::Hello hello = {0};
    size_t received = 0;
    auto result = objects.out_client.receive(&hello, codec::hello_size(), received);
    REQUIRE(!is_error(result));
    REQUIRE(received == codec::hello_size());
    REQUIRE(codec::validate_hello(hello));

    // Send wrong opcode back.
    static codec::Header hello_header = {0};
    hello_header.opcode = static_cast<char>(Opcode::soft_stop);
    size_t sent = 0;
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

    shutdown_client_server_test(objects);
}

//--------------------------
// Test - handshake timeout.
TEST_CASE("handshake timeout", "[arcus]") {}

//--------------------------------
// Message send and receive tests.

TEST_CASE("message send and receive", "[arcus]") {
    ClientServerTestObjects objects;
    constexpr size_t queue_length = 1024;
    init_client_server_test(objects, queue_length);
    handshake_client_server_test(objects);

    // Ensure no messages waiting anywhere.
    unsigned int count = 0;
    REQUIRE(!is_error(objects.server_connection->incoming_count(count)));
    REQUIRE(count == 0);
    REQUIRE(!is_error(objects.client_connection->incoming_count(count)));
    REQUIRE(count == 0);
    auto err = objects.server_connection->remove_incoming(
        [](const Message &) { return ONE_ERROR_NONE; });
    REQUIRE(err == ONE_ERROR_CONNECTION_QUEUE_EMPTY);
    err = objects.client_connection->remove_incoming(
        [](const Message &) { return ONE_ERROR_NONE; });
    REQUIRE(err == ONE_ERROR_CONNECTION_QUEUE_EMPTY);

    // Send a message from client to server.
    Message message;
    messages::prepare_soft_stop(1000, message);
    err = objects.client_connection->add_outgoing(message);

    const auto pump_messages = [&]() {
        for_sleep(10, 1, [&]() {
            REQUIRE(!is_error(objects.server_connection->update()));
            REQUIRE(!is_error(objects.client_connection->update()));
            return false;
        });
    };
    pump_messages();

    // Check it on server.
    REQUIRE(!is_error(objects.server_connection->incoming_count(count)));
    REQUIRE(count == 1);
    // Message pointer was consumed by client connection, can re-use var safely without
    // leak.
    err = objects.server_connection->remove_incoming([](const Message &message) {
        REQUIRE(message.code() == Opcode::soft_stop);
        int timeout = 0;
        REQUIRE(!is_error(message.payload().val_int("timeout", timeout)));
        REQUIRE(timeout == 1000);
        return ONE_ERROR_NONE;
    });
    REQUIRE(!is_error(err));

    // Fill up the outgoing messages.
    for (unsigned int i = 0; i < queue_length; ++i) {

        Message message;
        messages::prepare_soft_stop(1000, message);
        err = objects.client_connection->add_outgoing(message);
        REQUIRE(!is_error(err));
    }
    // Add one more message, which should not fit in the outgoing message queue.
    Array array;
    messages::prepare_allocated(array, message);
    err = objects.client_connection->add_outgoing(message);
    REQUIRE(err == ONE_ERROR_CONNECTION_OUTGOING_QUEUE_INSUFFICIENT_SPACE);

    // Ensure server received the initial messages and not the dropped one.
    pump_messages();
    REQUIRE(!is_error(objects.server_connection->incoming_count(count)));
    REQUIRE(count == queue_length);
    for (unsigned int i = 0; i < queue_length; ++i) {
        err = objects.server_connection->remove_incoming([](const Message &message) {
            REQUIRE(message.code() == Opcode::soft_stop);
            int timeout = 0;
            REQUIRE(!is_error(message.payload().val_int("timeout", timeout)));
            REQUIRE(timeout == 1000);
            return ONE_ERROR_NONE;
        });
        REQUIRE(!is_error(err));
    }

    shutdown_client_server_test(objects);
}

TEST_CASE("message send bad json", "[arcus]") {
    ClientServerTestObjects objects;
    constexpr size_t queue_length = 1024;
    init_client_server_test(objects, queue_length);
    handshake_client_server_test(objects);

    // Ensure no messages waiting anywhere.
    unsigned int count = 0;
    REQUIRE(!is_error(objects.server_connection->incoming_count(count)));
    REQUIRE(count == 0);
    REQUIRE(!is_error(objects.client_connection->incoming_count(count)));
    REQUIRE(count == 0);
    auto err = objects.server_connection->remove_incoming(
        [](const Message &) { return ONE_ERROR_NONE; });
    REQUIRE(err == ONE_ERROR_CONNECTION_QUEUE_EMPTY);
    err = objects.client_connection->remove_incoming(
        [](const Message &) { return ONE_ERROR_NONE; });
    REQUIRE(err == ONE_ERROR_CONNECTION_QUEUE_EMPTY);

    // Send a message from client to server.
    const std::string invalid_json = "{\"invalid_json\":true";
    Message message;
    err = message.init(Opcode::metadata, {invalid_json.c_str(), invalid_json.size()});
    err = objects.client_connection->add_outgoing(message);

    const auto pump_messages = [&]() {
        for_sleep(10, 1, [&]() {
            REQUIRE(!is_error(objects.server_connection->update()));
            auto error = objects.client_connection->update();

            if (is_error(error)) {
                REQUIRE(error == ONE_ERROR_CODEC_INVALID_HEADER);
                return true;
            }

            return false;
        });
    };

    pump_messages();

    // Check it on server.
    REQUIRE(!is_error(objects.server_connection->incoming_count(count)));
    REQUIRE(count == 0);

    shutdown_client_server_test(objects);
}
