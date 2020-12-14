#include <catch.hpp>
#include <util.h>

#include <iostream>
#include <map>
#include <thread>

#include <one/agent/agent.h>
#include <one/arcus/c_error.h>
#include <one/game/log.h>
#include <one/game/game.h>
#include <one/game/one_server_wrapper.h>

using namespace one_integration;
using namespace i3d::one;

void init_game(Game *game, unsigned int port) {
    game->init(port, 16, "name", "map", "mode", "version", seconds(0));
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
    for_sleep(100, 1, [&]() {
        auto err = agent->update();
        if (one_is_error(err)) {
            L_ERROR(one_error_text(err));
        }
        return false;
    });
}

void update_game_and_agent(Game *game, Agent *agent) {
    for_sleep(100, 1, [&]() {
        game->update();

        auto err = agent->update();
        if (one_is_error(err)) {
            L_ERROR(one_error_text(err));
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

    Game game;
    game.set_quiet(true);

    init_game(&game, port);
    update_game(&game);
    shutdown(&game);
}

TEST_CASE("two thread", "[concurrency]") {
    const unsigned int port = 19013;

    Game game;
    game.set_quiet(true);

    std::thread t1(init_game, &game, port);
    std::thread t2(init_game, &game, port);

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

    Game game;
    game.set_quiet(true);
    init_game(&game, port);

    Agent agent;
    init_agent(&agent, address, port);
    agent.set_quiet(true);

    pump_updates(10, 1, agent, game);

    for (int i = 0; i < 40; ++i) {
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

    Game game;
    game.set_quiet(true);

    std::thread t1(init_game, &game, port);
    std::thread t2(init_game, &game, port);

    Agent agent;
    std::thread t3(init_agent, &agent, address, port);

    t1.join();
    t2.join();
    t3.join();

    REQUIRE(agent.live_state_receive_count() == 0);
    REQUIRE(agent.host_information_send_count() == 0);
    REQUIRE(agent.application_instance_information_send_count() == 0);
    REQUIRE(agent.application_instance_status_receive_count() == 0);

    std::thread t4(update_game_send_statistics, &game);
    std::thread t5(update_game_send_statistics, &game);
    std::thread t6(update_game_and_agent, &game, &agent);

    t4.join();
    t5.join();
    t6.join();

    REQUIRE(agent.live_state_receive_count() > 0);
    REQUIRE(agent.host_information_send_count() > 0);
    REQUIRE(agent.application_instance_information_send_count() > 0);
    REQUIRE(agent.application_instance_status_receive_count() > 0);

    std::thread t7(shutdown, &game);
    std::thread t8(shutdown, &game);

    t7.join();
    t8.join();
}

TEST_CASE("multiple thread send information", "[concurrency]") {
    const unsigned int port = 19313;
    const auto address = "127.0.0.1";

    Game game;
    game.set_quiet(true);

    std::thread t1(init_game, &game, port);
    std::thread t2(init_game, &game, port);
    std::thread t3(init_game, &game, port);

    Agent agent;
    agent.set_quiet(true);

    std::thread t4(init_agent, &agent, address, port);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    REQUIRE(agent.live_state_receive_count() == 0);
    REQUIRE(agent.host_information_send_count() == 0);
    REQUIRE(agent.application_instance_information_send_count() == 0);
    REQUIRE(agent.application_instance_status_receive_count() == 0);

    std::thread t5(update_game_send_statistics, &game);
    std::thread t6(update_game_send_statistics, &game);
    std::thread t7(update_game_send_statistics, &game);
    std::thread t8(update_agent, &agent);

    t5.join();
    t6.join();
    t7.join();
    t8.join();

    REQUIRE(agent.live_state_receive_count() > 0);
    REQUIRE(agent.host_information_send_count() > 0);
    REQUIRE(agent.application_instance_information_send_count() > 0);
    REQUIRE(0 < agent.application_instance_status_receive_count());

    std::thread t9(shutdown, &game);
    std::thread t10(shutdown, &game);
    std::thread t11(shutdown, &game);

    t9.join();
    t10.join();
    t11.join();
}

TEST_CASE("two game on same process", "[concurrency]") {
    const unsigned int port = 19413;
    const unsigned int port2 = 19414;
    const auto address = "127.0.0.1";

    Game game;
    Game game2;
    game.set_quiet(true);
    game2.set_quiet(true);

    init_game(&game, port);
    init_game(&game2, port2);

    Agent agent;
    Agent agent2;
    agent.set_quiet(true);
    agent2.set_quiet(true);

    init_agent(&agent, address, port);
    init_agent(&agent2, address, port2);

    REQUIRE(agent.live_state_receive_count() == 0);
    REQUIRE(agent.host_information_send_count() == 0);
    REQUIRE(agent.application_instance_information_send_count() == 0);
    REQUIRE(agent.application_instance_status_receive_count() == 0);

    REQUIRE(agent2.live_state_receive_count() == 0);
    REQUIRE(agent2.host_information_send_count() == 0);
    REQUIRE(agent2.application_instance_information_send_count() == 0);
    REQUIRE(agent2.application_instance_status_receive_count() == 0);

    wait_until (200, [&]() {
        game.update();
        game2.update();
        agent.update();
        agent2.update();
        return game.one_server_wrapper().status() == OneServerWrapper::Status::ready &&
           game2.one_server_wrapper().status() == OneServerWrapper::Status::ready;
    });

    for_sleep(100, 1, [&]() {
        game.alter_game_state();
        game.update();
        agent.update();

        game2.alter_game_state();
        game2.update();
        agent2.update();

        return false;
    });

    REQUIRE(agent.live_state_receive_count() > 0);
    REQUIRE(agent.host_information_send_count() > 0);
    REQUIRE(agent.application_instance_information_send_count() > 0);
    REQUIRE(agent.application_instance_status_receive_count() > 0);

    REQUIRE(0 < agent2.live_state_receive_count());
    REQUIRE(agent2.host_information_send_count() == 1);
    REQUIRE(agent2.application_instance_information_send_count() == 1);
    REQUIRE(0 < agent2.application_instance_status_receive_count());

    game.shutdown();
    game2.shutdown();
}

TEST_CASE("multiple game on the same process", "[concurrency]") {
    const unsigned int port = 19513;
    const unsigned int port2 = 19514;
    const unsigned int port3 = 19515;
    const auto address = "127.0.0.1";

    Game game;
    Game game2;
    Game game3;
    game.set_quiet(true);
    game2.set_quiet(true);
    game3.set_quiet(true);
    init_game(&game, port);
    init_game(&game2, port2);
    init_game(&game3, port3);

    Agent agent;
    Agent agent2;
    Agent agent3;
    agent.set_quiet(true);
    agent2.set_quiet(true);
    agent3.set_quiet(true);
    init_agent(&agent, address, port);
    init_agent(&agent2, address, port2);
    init_agent(&agent3, address, port3);

    REQUIRE(agent.live_state_receive_count() == 0);
    REQUIRE(agent.host_information_send_count() == 0);
    REQUIRE(agent.application_instance_information_send_count() == 0);
    REQUIRE(agent.application_instance_status_receive_count() == 0);

    REQUIRE(agent2.live_state_receive_count() == 0);
    REQUIRE(agent2.host_information_send_count() == 0);
    REQUIRE(agent2.application_instance_information_send_count() == 0);
    REQUIRE(agent2.application_instance_status_receive_count() == 0);

    REQUIRE(agent3.live_state_receive_count() == 0);
    REQUIRE(agent3.host_information_send_count() == 0);
    REQUIRE(agent3.application_instance_information_send_count() == 0);
    REQUIRE(agent3.application_instance_status_receive_count() == 0);

    wait_until (200, [&]() {
        game.update();
        game2.update();
        game3.update();
        agent.update();
        agent2.update();
        agent3.update();
        return game.one_server_wrapper().status() == OneServerWrapper::Status::ready &&
           game2.one_server_wrapper().status() == OneServerWrapper::Status::ready &&
           game3.one_server_wrapper().status() == OneServerWrapper::Status::ready;
    });

    for_sleep(100, 1, [&]() {
        game.alter_game_state();
        game.update();

        game2.alter_game_state();
        game2.update();

        game3.alter_game_state();
        game3.update();

        agent.update();
        agent2.update();
        agent3.update();
        return false;
    });

    REQUIRE(agent.live_state_receive_count() > 0);
    REQUIRE(agent.host_information_send_count() == 1);
    REQUIRE(agent.application_instance_information_send_count() > 0);
    REQUIRE(agent.application_instance_status_receive_count() > 0);

    REQUIRE(agent2.live_state_receive_count() > 0);
    REQUIRE(agent2.host_information_send_count() == 1);
    REQUIRE(agent2.application_instance_information_send_count() == 1);
    REQUIRE(agent2.application_instance_status_receive_count() > 0);

    REQUIRE(agent3.live_state_receive_count() > 0);
    REQUIRE(agent3.host_information_send_count() == 1);
    REQUIRE(agent3.application_instance_information_send_count() == 1);
    REQUIRE(0 < agent3.application_instance_status_receive_count());

    game.shutdown();
    game2.shutdown();
    game3.shutdown();
}
