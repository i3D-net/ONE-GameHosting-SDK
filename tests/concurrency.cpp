#include <catch.hpp>
#include <util.h>

#include <one/agent/agent.h>
#include <one/arcus/error.h>
#include <one/game/log.h>
#include <one/game/game.h>
#include <one/game/one_server_wrapper.h>

#include <iostream>
#include <map>

#include <thread>

using namespace game;
using namespace i3d::one;

void init_game(Game *game) {
    game->init(16, "name", "map", "mode", "version");
}

void init_agent(Agent *agent, const char *address, int port) {
    agent->init(address, port);
}

void shutdown(Game *game) {
    game->shutdown();
}

void update_game(Game *game) {
    for (int i = 0; i < 1000; ++i) {
        game->update();
    }
}

void update_agent(Agent *agent) {
    for (int i = 0; i < 1000; ++i) {
        auto err = agent->update();
        if (is_error(err)) {
            L_ERROR(error_text(err));
        }
    }
}

void update_game_send_statistics(Game *game) {
    for (int i = 0; i < 10; ++i) {
        auto &wrapper = game->one_server_wrapper();
        const auto &status = wrapper.game_state();
        auto new_state = status;

        if (status.max_players < status.players + 1) {
            new_state.players = 0;
        } else {
            new_state.players++;
        }

        wrapper.set_game_state(new_state);
        game->update();
    }
}

TEST_CASE("single thread", "[concurrency]") {
    const unsigned int port = 19012;

    L_INFO("single thread check: START");

    Game game(port);

    init_game(&game);
    update_game(&game);
    shutdown(&game);

    L_INFO("single thread check: END");
}

TEST_CASE("two thread", "[concurrency]") {
    const unsigned int port = 19013;

    L_INFO("two thread check: START");

    Game game(port);

    std::thread t1(init_game, &game);
    std::thread t2(init_game, &game);

    t1.join();
    t2.join();

    std::thread t3(update_game, &game);
    std::thread t4(update_game, &game);

    t3.join();
    t4.join();

    std::thread t5(shutdown, &game);
    std::thread t6(shutdown, &game);

    t5.join();
    t6.join();

    L_INFO("two thread check: END");
}

TEST_CASE("single thread send information", "[concurrency]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19112;

    L_INFO("single thread send information: START");

    Game game(port);
    init_game(&game);

    Agent agent;
    REQUIRE(!is_error(agent.init(address, port)));

    pump_updates(10, 1, agent, game);

    for (int i = 0; i < 100; ++i) {
        update_game_send_statistics(&game);
        update_agent(&agent);
    }

    shutdown(&game);

    L_INFO("single thread send information: END");
}

TEST_CASE("two thread send information", "[concurrency]") {
    const unsigned int port = 19213;
    const auto address = "127.0.0.1";

    L_INFO("two thread send information: START");

    Game game(port);

    std::thread t1(init_game, &game);
    std::thread t2(init_game, &game);

    Agent agent;
    std::thread t3(init_agent, &agent, address, port);

    t1.join();
    t2.join();
    t3.join();

    std::thread t4(update_game_send_statistics, &game);
    std::thread t5(update_game_send_statistics, &game);
    std::thread t6(update_agent, &agent);

    t4.join();
    t5.join();
    t6.join();

    std::thread t7(shutdown, &game);
    std::thread t8(shutdown, &game);

    t7.join();
    t8.join();

    L_INFO("two thread send information: END");
}

TEST_CASE("multiple thread send information", "[concurrency]") {
    const unsigned int port = 19313;
    const auto address = "127.0.0.1";

    L_INFO("two thread send information: START");

    Game game(port);

    std::thread t1(init_game, &game);
    std::thread t2(init_game, &game);
    std::thread t3(init_game, &game);

    Agent agent;

    std::thread t4(init_agent, &agent, address, port);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::thread t5(update_game_send_statistics, &game);
    std::thread t6(update_game_send_statistics, &game);
    std::thread t7(update_game_send_statistics, &game);
    std::thread t8(update_agent, &agent);

    t5.join();
    t6.join();
    t7.join();
    t8.join();

    std::thread t9(shutdown, &game);
    std::thread t10(shutdown, &game);

    t9.join();
    t10.join();

    L_INFO("two thread send information: END");
}
