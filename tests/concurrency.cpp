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
    agent->set_quiet(true);
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
    for_sleep(1000, 1, [&]() {
        auto err = agent->update();
        if (is_error(err)) {
            L_ERROR(error_text(err));
        }
        return false;
    });
}

void update_game_and_agent(Game *game, Agent *agent) {
    for_sleep(100, 1, [&]() {
        game->update();

        auto err = agent->update();
        if (is_error(err)) {
            L_ERROR(error_text(err));
        }

        return false;
    });
}

void update_game_send_statistics(Game *game) {
    while (game->one_server_wrapper().status() != OneServerWrapper::Status::ready) {
        game->update();
    }

    for_sleep(10, 0, [&]() {
        game->alter_game_state();
        game->update();
        return false;
    });
}

TEST_CASE("single thread", "[concurrency]") {
    const unsigned int port = 19012;

    Game game(port);
    game.set_quiet(true);

    init_game(&game);
    update_game(&game);
    shutdown(&game);
}

TEST_CASE("two thread", "[concurrency]") {
    const unsigned int port = 19013;

    Game game(port);
    game.set_quiet(true);

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
}

TEST_CASE("long:single thread send information", "[concurrency]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 19112;

    Game game(port);
    game.set_quiet(true);
    init_game(&game);

    Agent agent;
    init_agent(&agent, address, port);
    agent.set_quiet(true);

    pump_updates(10, 1, agent, game);

    for (int i = 0; i < 100; ++i) {
        update_game_send_statistics(&game);
        update_agent(&agent);
    }

    shutdown(&game);
}

// Left without long tag since it is relatively fast and good to run
// at least one threading test per build.
TEST_CASE("two thread send information", "[concurrency]") {
    const unsigned int port = 19213;
    const auto address = "127.0.0.1";

    Game game(port);
    game.set_quiet(true);

    std::thread t1(init_game, &game);
    std::thread t2(init_game, &game);

    Agent agent;
    std::thread t3(init_agent, &agent, address, port);

    t1.join();
    t2.join();
    t3.join();

    REQUIRE(agent.live_state_call_count() == 0);
    REQUIRE(agent.host_information_call_count() == 0);
    REQUIRE(agent.application_instance_information_call_count() == 0);
    REQUIRE(agent.application_instance_get_status_call_count() == 0);
    REQUIRE(agent.application_instance_set_status_call_count() == 0);

    std::thread t4(update_game_send_statistics, &game);
    std::thread t5(update_game_send_statistics, &game);
    std::thread t6(update_game_and_agent, &game, &agent);

    t4.join();
    t5.join();
    t6.join();

    REQUIRE(0 < agent.live_state_call_count());
    REQUIRE(agent.host_information_call_count() == 1);
    REQUIRE(agent.application_instance_information_call_count() == 1);
    REQUIRE(0 < agent.application_instance_get_status_call_count());
    REQUIRE(agent.application_instance_set_status_call_count() == 3);

    std::thread t7(shutdown, &game);
    std::thread t8(shutdown, &game);

    t7.join();
    t8.join();
}

TEST_CASE("long:multiple thread send information", "[concurrency]") {
    const unsigned int port = 19313;
    const auto address = "127.0.0.1";

    Game game(port);
    game.set_quiet(true);

    std::thread t1(init_game, &game);
    std::thread t2(init_game, &game);
    std::thread t3(init_game, &game);

    Agent agent;
    agent.set_quiet(true);

    std::thread t4(init_agent, &agent, address, port);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    REQUIRE(agent.live_state_call_count() == 0);
    REQUIRE(agent.host_information_call_count() == 0);
    REQUIRE(agent.application_instance_information_call_count() == 0);
    REQUIRE(agent.application_instance_get_status_call_count() == 0);
    REQUIRE(agent.application_instance_set_status_call_count() == 0);

    std::thread t5(update_game_send_statistics, &game);
    std::thread t6(update_game_send_statistics, &game);
    std::thread t7(update_game_send_statistics, &game);
    std::thread t8(update_agent, &agent);

    t5.join();
    t6.join();
    t7.join();
    t8.join();

    REQUIRE(0 < agent.live_state_call_count());
    REQUIRE(agent.host_information_call_count() == 1);
    REQUIRE(agent.application_instance_information_call_count() == 1);
    REQUIRE(0 < agent.application_instance_get_status_call_count());
    REQUIRE(agent.application_instance_set_status_call_count() == 3);

    std::thread t9(shutdown, &game);
    std::thread t10(shutdown, &game);

    t9.join();
    t10.join();
}
