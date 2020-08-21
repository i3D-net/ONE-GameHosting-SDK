#include <catch.hpp>
#include <util.h>

#include <one/arcus/error.h>
#include <one/arcus/internal/codec.h>
#include <one/arcus/internal/socket.h>
#include <one/arcus/internal/connection.h>
#include <one/game/game.h>

using namespace game;
using namespace one;

TEST_CASE("life cycle", "[fake game]") {
    Game game(19001, 1, 10, 54, "test game", "test map", "test mode", "test version");
    REQUIRE(game.init(1024, 1024) == 0);
    REQUIRE(game.shutdown() == 0);
}

TEST_CASE("connection error handling", "[fake game]") {
    Game game(19001, 1, 10, 54, "test game", "test map", "test mode", "test version");
    REQUIRE(game.init(1024, 1024) == 0);

    // Connect a client socket to fake a scriptable agent.
    Socket client;
    client.init();
    REQUIRE(!is_error(client.connect("127.0.0.1", 19001)));

    for_sleep(5, 1, [&]() {
        REQUIRE(game.update() == 0);
        return false;
    });

    // Receive hello from server.
    codec::Hello hello = {0};
    size_t received = 0;
    auto result = client.receive(&hello, codec::hello_size(), received);
    REQUIRE(!is_error(result));
    REQUIRE(received == codec::hello_size());
    REQUIRE(codec::validate_hello(hello));

    REQUIRE(game.shutdown() == 0);
}
