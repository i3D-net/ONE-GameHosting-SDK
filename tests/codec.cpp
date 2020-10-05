#include <catch.hpp>

#include <functional>

#include <one/arcus/error.h>
#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/internal/message.h>
#include <one/arcus/message.h>
#include <one/arcus/opcode.h>

#include <array>
#include <string>

using namespace i3d::one;

TEST_CASE("max size matching the connection socket buffer", "[codec]") {
    REQUIRE(codec::header_size() + codec::payload_max_size() <=
            connection::stream_send_buffer_size());
    REQUIRE(codec::header_size() + codec::payload_max_size() <=
            connection::stream_receive_buffer_size());
}

TEST_CASE("hello", "[codec]") {
    {
        auto hello = codec::valid_hello();
        REQUIRE(validate_hello(hello));
    }
    {
        auto hello = codec::valid_hello();
        hello.version = (char)0x0;
        REQUIRE(!validate_hello(hello));
    }
    {
        auto hello = codec::valid_hello();
        hello.dummy = (char)0x1;
        REQUIRE(!validate_hello(hello));
    }
    {
        auto hello = codec::valid_hello();
        hello.id[0] = (char)0x0;
        REQUIRE(!validate_hello(hello));
    }

    // No Header validation in the Codec.
}

TEST_CASE("header", "[codec]") {
    codec::Header header = {0};
    header.flags = (char)0x0;
    header.opcode = (char)Opcode::metadata;

    REQUIRE(codec::validate_header(header));
    std::array<char, codec::header_size()> data;
    REQUIRE(!is_error(header_to_data(header, data)));

    header.flags = (char)0x1;
    REQUIRE(!codec::validate_header(header));
    REQUIRE(is_error(header_to_data(header, data)));

    header.flags = (char)0x0;
    header.opcode = (char)Opcode::invalid;
    REQUIRE(!codec::validate_header(header));
    REQUIRE(is_error(header_to_data(header, data)));

    header.flags = (char)0x0;
    header.opcode = (char)Opcode::metadata;

    REQUIRE(!is_error(header_to_data(header, data)));

    codec::Header new_header = {0};
    REQUIRE(is_error(data_to_header(data.data(), codec::header_size() - 1, new_header)));
    REQUIRE(is_error(data_to_header(data.data(), codec::header_size() + 1, new_header)));
    REQUIRE(!is_error(data_to_header(data.data(), codec::header_size(), new_header)));
    REQUIRE(new_header.flags == header.flags);
    REQUIRE(new_header.opcode == header.opcode);
}

TEST_CASE("payload", "[codec]") {
    Payload payload;
    std::string json = "{}";
    REQUIRE(!is_error(payload.from_json({json.c_str(), json.size()})));

    std::array<char, codec::payload_max_size()> data;
    size_t payload_length = 0;
    REQUIRE(!is_error(codec::payload_to_data(payload, payload_length, data)));
    REQUIRE(payload_length == 0);
    REQUIRE(std::string(data.data(), payload_length) == "");
    Payload new_payload;
    REQUIRE(!is_error(codec::data_to_payload(data.data(), payload_length, new_payload)));
    REQUIRE(payload.get() == new_payload.get());

    json = "{\"timeout\":1000}";
    REQUIRE(!is_error(payload.from_json({json.c_str(), json.size()})));
    REQUIRE(!is_error(codec::payload_to_data(payload, payload_length, data)));
    REQUIRE(payload_length == json.size());
    REQUIRE(std::string(data.data(), payload_length) == json);

    json =
        "{\"TableHeight\":{\"tag\":\"00181130\",\"value\":163.253006,\"vr\":\"DS\"},"
        "\"TransferSyntaxUID\":{\"tag\":\"00020010\",\"value\":\"1.2.840.10008.1.2\","
        "\"vr\":\"UI\"},\"VerifyingObserverName\":{\"alphabetic\":true,\"tag\":"
        "\"0040A075\",\"value\":\"Removed by "
        "CTP\",\"vr\":\"PN\"},\"WindowCenter\":{\"tag\":\"00281050\",\"value\":40,\"vr\":"
        "\"DS\"},\"WindowWidth\":{\"tag\":\"00281051\",\"value\":400,\"vr\":\"DS\"},"
        "\"XRayTubeCurrent\":{\"tag\":\"00181151\",\"value\":160,\"vr\":\"IS\"},\"tag_"
        "00130010\":{\"tag\":\"00130010\",\"value\":\"CTP\",\"vr\":\"LO\"}}";
    REQUIRE(!is_error(payload.from_json({json.c_str(), json.size()})));
    REQUIRE(!is_error(codec::payload_to_data(payload, payload_length, data)));
    REQUIRE(payload_length == json.size());
    REQUIRE(std::string(data.data(), payload_length) == json);
    REQUIRE(!is_error(codec::data_to_payload(data.data(), payload_length, new_payload)));
    REQUIRE(payload.get() == new_payload.get());
}

TEST_CASE("message empty payload consistency", "[codec]") {
    codec::Header header = {0};
    Message message;
    Message new_message;
    size_t data_length = 0;
    size_t data_read = 0;
    std::array<char, codec::header_size() + codec::payload_max_size()> data;
    uint32_t packet_id = 0;

    {  // Health message: because it has an empty payload.
        message.init(Opcode::health, Payload());
        data_length = 0;
        data_read = 0;
        header = {0};
        REQUIRE(
            !is_error(codec::message_to_data(++packet_id, message, data_length, data)));
        REQUIRE(!is_error(codec::data_to_message(data.data(), data_length, data_read,
                                                 header, new_message)));
        REQUIRE(data_length == data_read);
        REQUIRE(data_length == codec::header_size());
        REQUIRE(header.length == 0);
        REQUIRE((Opcode)header.opcode == Opcode::health);
        REQUIRE(message.code() == Opcode::health);
        REQUIRE(new_message.code() == message.code());
        REQUIRE(new_message.payload().is_empty());
    }
}

TEST_CASE("message", "[codec]") {
    codec::Header header = {0};
    Message message;
    Message new_message;
    size_t data_length = 0;
    size_t data_read = 0;
    std::array<char, codec::header_size() + codec::payload_max_size()> data;
    uint32_t packet_id = 0;

    {  // soft stop request
        REQUIRE(!is_error(messages::prepare_soft_stop(1000, message)));
        data_length = 0;
        data_read = 0;
        header = {0};
        REQUIRE(
            !is_error(codec::message_to_data(++packet_id, message, data_length, data)));
        REQUIRE(!is_error(codec::data_to_message(data.data(), data_length, data_read,
                                                 header, new_message)));
        REQUIRE(data_length == data_read);
        REQUIRE((Opcode)header.opcode == Opcode::soft_stop);
        REQUIRE(message.code() == Opcode::soft_stop);
        REQUIRE(new_message.code() == message.code());
        REQUIRE(new_message.payload().get() == message.payload().get());
        int timeout = 0;
        REQUIRE(!is_error(message.payload().val_int("timeout", timeout)));
        REQUIRE(timeout == 1000);
    }

    {  // allocated request
        Array array;
        array.push_back_int(1000);
        REQUIRE(!is_error(messages::prepare_allocated(array, message)));
        data_length = 0;
        data_read = 0;
        header = {0};
        REQUIRE(
            !is_error(codec::message_to_data(++packet_id, message, data_length, data)));
        REQUIRE(!is_error(codec::data_to_message(data.data(), data_length, data_read,
                                                 header, new_message)));
        REQUIRE(data_length == data_read);
        REQUIRE((Opcode)header.opcode == Opcode::allocated);
        REQUIRE(message.code() == Opcode::allocated);
        REQUIRE(new_message.code() == message.code());
        REQUIRE(new_message.payload().get() == message.payload().get());
        Array new_array;
        REQUIRE(!is_error(message.payload().val_array("data", new_array)));
        REQUIRE(new_array.get() == array.get());
    }

    {  // meta data request
        Array array;
        array.push_back_int(1000);
        REQUIRE(!is_error(messages::prepare_metadata(array, message)));
        data_length = 0;
        data_read = 0;
        header = {0};
        REQUIRE(
            !is_error(codec::message_to_data(++packet_id, message, data_length, data)));
        REQUIRE(!is_error(codec::data_to_message(data.data(), data_length, data_read,
                                                 header, new_message)));
        REQUIRE(data_length == data_read);
        REQUIRE((Opcode)header.opcode == Opcode::metadata);
        REQUIRE(message.code() == Opcode::metadata);
        REQUIRE(new_message.code() == message.code());
        REQUIRE(new_message.payload().get() == message.payload().get());
        Array new_array;
        REQUIRE(!is_error(message.payload().val_array("data", new_array)));
        REQUIRE(new_array.get() == array.get());
    }

    {  // live state response
        REQUIRE(!is_error(messages::prepare_live_state(
            1, 16, "name test", "map test", "mode test", "version test", message)));
        data_length = 0;
        data_read = 0;
        header = {0};
        REQUIRE(
            !is_error(codec::message_to_data(++packet_id, message, data_length, data)));
        REQUIRE(!is_error(codec::data_to_message(data.data(), data_length, data_read,
                                                 header, new_message)));
        REQUIRE(data_length == data_read);
        REQUIRE((Opcode)header.opcode == Opcode::live_state);
        REQUIRE(message.code() == Opcode::live_state);
        REQUIRE(new_message.code() == message.code());
        REQUIRE(new_message.payload().get() == message.payload().get());
        int players = 0;
        REQUIRE(!is_error(message.payload().val_int("players", players)));
        REQUIRE(players == 1);
        int max_players = 0;
        REQUIRE(!is_error(message.payload().val_int("maxPlayers", max_players)));
        REQUIRE(max_players == 16);
        std::string name;
        REQUIRE(!is_error(message.payload().val_string("name", name)));
        REQUIRE(name == "name test");
        std::string map;
        REQUIRE(!is_error(message.payload().val_string("map", map)));
        REQUIRE(map == "map test");
        std::string mode;
        REQUIRE(!is_error(message.payload().val_string("mode", mode)));
        REQUIRE(mode == "mode test");
        std::string version;
        REQUIRE(!is_error(message.payload().val_string("version", version)));
        REQUIRE(version == "version test");
    }

    {  // application instance set status request
        REQUIRE(!is_error(
            messages::prepare_application_instance_set_status_request(4, message)));
        data_length = 0;
        data_read = 0;
        header = {0};
        REQUIRE(
            !is_error(codec::message_to_data(++packet_id, message, data_length, data)));
        REQUIRE(!is_error(codec::data_to_message(data.data(), data_length, data_read,
                                                 header, new_message)));
        REQUIRE(data_length == data_read);
        REQUIRE((Opcode)header.opcode == Opcode::application_instance_set_status_request);
        REQUIRE(message.code() == Opcode::application_instance_set_status_request);
        REQUIRE(new_message.code() == message.code());
        REQUIRE(new_message.payload().get() == message.payload().get());
        int status = 0;
        REQUIRE(!is_error(message.payload().val_int("status", status)));
        REQUIRE(status == 4);
    }
}
