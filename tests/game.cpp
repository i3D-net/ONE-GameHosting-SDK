#include <catch.hpp>
#include <util.h>

#include <one/agent/agent.h>
#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/internal/connection.h>
#include <one/arcus/object.h>
#include <one/game/game.h>
#include <one/game/log.h>

using namespace game;
using namespace i3d::one;

bool soft_stop_callback_has_been_called = false;
bool allocated_callback_has_been_called = false;
bool meta_data_callback_has_been_called = false;

void soft_stop_callback(int timeout) {
    L_INFO("soft stop payload:");
    L_INFO("timeout:" + std::to_string(timeout));
    soft_stop_callback_has_been_called = true;
}

void allocated_callback(OneServerWrapper::AllocatedData data) {
    L_INFO("allocated payload:");
    L_INFO("max_players:" + data.max_players);
    L_INFO("map:" + data.map);
    allocated_callback_has_been_called = true;
}

void meta_data_callback(OneServerWrapper::MetaDataData data) {
    L_INFO("allocated payload:");
    L_INFO("map:" + data.map);
    L_INFO("mode:" + data.mode);
    L_INFO("type:" + data.type);
    allocated_callback_has_been_called = true;
}

TEST_CASE("life cycle", "[fake game]") {
    Game game(19001);
    REQUIRE(game.init(10, 54, "test game", "test map", "test mode", "test version"));
    game.shutdown();
}

TEST_CASE("connection error handling", "[fake game]") {
    Game game(19001);
    REQUIRE(game.init(10, 54, "test game", "test map", "test mode", "test version"));

    // Connect a client socket to fake a scriptable agent.
    Socket client;
    client.init();
    REQUIRE(!is_error(client.connect("127.0.0.1", 19001)));

    // Update game so that connection starts and handhsake is sent.
    auto pump_game = [&]() {
        for_sleep(5, 1, [&]() {
            game.update();
            return false;
        });
    };
    pump_game();

    REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::handshake);

    // Receive hello from server.
    codec::Hello hello = {0};
    size_t received = 0;
    auto result = client.receive(&hello, codec::hello_size(), received);
    REQUIRE(!is_error(result));
    REQUIRE(received == codec::hello_size());
    REQUIRE(codec::validate_hello(hello));

    // Send wrong opcode back.
    static codec::Header hello_header = {0};
    hello_header.opcode = static_cast<char>(Opcode::soft_stop_request);
    size_t sent = 0;
    result = client.send(&hello_header, codec::header_size(), sent);
    REQUIRE(!is_error(result));
    REQUIRE(sent == codec::header_size());

    // Update the game, at some point it should error while trying to process
    // the handshake.
    for_sleep(5, 1, [&]() {
        game.update();
        return false;
    });

    pump_game();
    REQUIRE(game.one_server_wrapper().status() ==
            OneServerWrapper::Status::waiting_for_client);

    game.shutdown();
}

TEST_CASE("ancillary message payload parsing", "[fake game]") {
    {  // Allocated payload.
        const std::string map_str = "islands_large";
        const std::string max_player_str = "16";

        Object map;
        auto err = map.set_val_string("key", "map");
        REQUIRE(!is_error(err));
        err = map.set_val_string("value", map_str);
        REQUIRE(!is_error(err));

        Object max_player;
        err = max_player.set_val_string("key", "maxPlayer");
        REQUIRE(!is_error(err));
        err = max_player.set_val_string("value", max_player_str);
        REQUIRE(!is_error(err));

        Array data;

        OneArrayPtr ptr = (OneArray *)&data;
        OneServerWrapper::AllocatedData allocated;
        REQUIRE(!TestOneServerWrapper::extract_allocated_payload(ptr, allocated));

        data.push_back_object(map);
        REQUIRE(!TestOneServerWrapper::extract_allocated_payload(ptr, allocated));

        data.push_back_object(max_player);
        REQUIRE(TestOneServerWrapper::extract_allocated_payload(ptr, allocated));
        REQUIRE(!is_error(err));
        REQUIRE(std::strncmp(allocated.map.c_str(), map_str.c_str(), map_str.size()) ==
                0);
        REQUIRE(std::strncmp(allocated.max_players.c_str(), max_player_str.c_str(),
                             max_player_str.size()) == 0);
    }

    {  // MetaData payload.
        const std::string map_str = "islands_large";
        const std::string mode_str = "BR";
        const std::string type_str = "squads";

        Object map;
        auto err = map.set_val_string("key", "map");
        REQUIRE(!is_error(err));
        err = map.set_val_string("value", map_str);
        REQUIRE(!is_error(err));

        Object mode;
        err = mode.set_val_string("key", "mode");
        REQUIRE(!is_error(err));
        err = mode.set_val_string("value", mode_str);
        REQUIRE(!is_error(err));

        Object type;
        err = type.set_val_string("key", "type");
        REQUIRE(!is_error(err));
        err = type.set_val_string("value", type_str);
        REQUIRE(!is_error(err));

        Array data;

        OneArrayPtr ptr = (OneArray *)&data;
        OneServerWrapper::MetaDataData meta_data;
        REQUIRE(!TestOneServerWrapper::extract_meta_data_payload(ptr, meta_data));

        data.push_back_object(map);
        REQUIRE(!TestOneServerWrapper::extract_meta_data_payload(ptr, meta_data));

        data.push_back_object(mode);
        REQUIRE(!TestOneServerWrapper::extract_meta_data_payload(ptr, meta_data));

        data.push_back_object(type);
        REQUIRE(TestOneServerWrapper::extract_meta_data_payload(ptr, meta_data));
        REQUIRE(!is_error(err));
        REQUIRE(std::strncmp(meta_data.map.c_str(), map_str.c_str(), map_str.size()) ==
                0);
        REQUIRE(std::strncmp(meta_data.mode.c_str(), mode_str.c_str(), mode_str.size()) ==
                0);
        REQUIRE(std::strncmp(meta_data.type.c_str(), type_str.c_str(), type_str.size()) ==
                0);
    }
}

TEST_CASE("Agent connects to a game & send requests", "[fake game]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19002;

    Game game(port);
    REQUIRE(game.init(1, 16, "name", "map", "mode", "version"));
    REQUIRE(game.one_server_wrapper().status() ==
            OneServerWrapper::Status::waiting_for_client);

    Agent agent;
    REQUIRE(agent.init(address, port) == 0);
    REQUIRE(agent.update() == ONE_ERROR_NONE);
    REQUIRE(agent.client().status() == Client::Status::handshake);

    pump_updates(10, 1, agent, game);
    REQUIRE(game.one_server_wrapper().status() == OneServerWrapper::Status::ready);
    REQUIRE(agent.client().status() == Client::Status::ready);

    auto &wrapper = game.one_server_wrapper();
    wrapper.set_soft_stop_callback(soft_stop_callback);
    wrapper.set_allocated_callback(allocated_callback);
    wrapper.set_meta_data_callback(meta_data_callback);

    {
        REQUIRE(agent.send_soft_stop_request(1000) == 0);
        agent.update();
        game.update();
        REQUIRE(soft_stop_callback_has_been_called == true);
    }

    return;

    // allocated_request
    {
        Object map;
        auto err = map.set_val_string("key", "map");
        REQUIRE(!is_error(err));
        err = map.set_val_string("value", "islands_large");
        REQUIRE(!is_error(err));

        Object max_player;
        err = max_player.set_val_string("key", "maxPlayer");
        REQUIRE(!is_error(err));
        err = max_player.set_val_string("value", "16");
        REQUIRE(!is_error(err));

        Array data;
        data.push_back_object(map);
        data.push_back_object(max_player);

        REQUIRE(agent.send_allocated_request(&data) == 0);
        agent.update();
        game.update();
        REQUIRE(allocated_callback_has_been_called == true);
    }

    // meta_data_request
    {
        Object map;
        auto err = map.set_val_string("key", "map");
        REQUIRE(!is_error(err));
        err = map.set_val_string("value", "islands_large");
        REQUIRE(!is_error(err));

        Object mode;
        err = mode.set_val_string("key", "mode");
        REQUIRE(!is_error(err));
        err = mode.set_val_string("value", "BR");
        REQUIRE(!is_error(err));

        Object type;
        err = type.set_val_string("key", "type");
        REQUIRE(!is_error(err));
        err = type.set_val_string("value", "squads");
        REQUIRE(!is_error(err));

        Array data;
        data.push_back_object(map);
        data.push_back_object(mode);
        data.push_back_object(type);

        Array array;
        REQUIRE(agent.send_meta_data_request(&data) == 0);
        agent.update();
        game.update();
        REQUIRE(meta_data_callback_has_been_called == true);
    }

    game.shutdown();
}
