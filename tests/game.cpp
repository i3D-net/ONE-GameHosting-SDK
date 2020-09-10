#include <catch.hpp>
#include <util.h>

#include <one/arcus/error.h>
#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/internal/connection.h>
#include <one/game/game.h>

using namespace game;
using namespace i3d::one;

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
