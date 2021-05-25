#include <catch.hpp>
#include <tests/one/arcus/util.h>

#include <thread>

#include <tests/one/arcus/harness.h>
#include <one/fake/arcus/game/log.h>

using namespace one_integration;
using namespace i3d::one;
using namespace std::chrono;

void run(StressHarness *harness, seconds duration, milliseconds sleep) {
    harness->run(duration, sleep);
}

TEST_CASE("soak:test high usage over a long period", "[stress]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 18000;
    StressHarness stress(address, port);

    stress.set_stress_game_callback([](Game &game, int random) {
        for (int i = 0; i < random; ++i) {
            game.alter_game_state();
        }
        return ONE_ERROR_NONE;
    });

    stress.set_stress_agent_callback(
        [](Agent &agent, int random) { return agent.send_soft_stop(random); });

    stress.run(seconds(60), milliseconds(0));

    auto &agent = stress.agent();
    auto &game = stress.game();

    REQUIRE(game.soft_stop_receive_count() > 0);
    REQUIRE(agent.live_state_receive_count() > 0);

    L_INFO("soak:test high usage over a long period");
    L_INFO("Agent received: " + std::to_string(agent.live_state_receive_count()) +
           " live_state messages");
    L_INFO("Game received: " + std::to_string(game.soft_stop_receive_count()) +
           " soft_stop messages");

    stress.log_agent_error_tally();
    stress.log_game_error_tally();
    REQUIRE(true);
}

TEST_CASE("soak:test very high usage over a long period", "[stress]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 18000;
    StressHarness stress(address, port);

    stress.set_stress_game_callback([](Game &game, int random) {
        for (int i = 0; i < random; ++i) {
            game.alter_game_state();
        }
        return ONE_ERROR_NONE;
    });

    stress.set_stress_agent_callback([](Agent &agent, int random) {
        for (int i = 0; i < random; ++i) {
            auto err = agent.send_soft_stop(random);
            if (is_error(err)) {
                return err;
            }
        }

        return ONE_ERROR_NONE;
    });

    stress.run(seconds(60), milliseconds(0));

    auto &agent = stress.agent();
    auto &game = stress.game();

    REQUIRE(0 < game.soft_stop_receive_count());
    REQUIRE(0 < agent.live_state_receive_count());

    L_INFO("soak:test very high usage over a long period");
    L_INFO("Agent received: " + std::to_string(agent.live_state_receive_count()) +
           " live_state messages");
    L_INFO("Game received: " + std::to_string(game.soft_stop_receive_count()) +
           " soft_stop messages");

    stress.log_agent_error_tally();
    stress.log_game_error_tally();
    REQUIRE(true);
}

TEST_CASE("soak:test high usage over a long period on multiple threads", "[stress]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 18000;
    StressHarness stress(address, port);

    stress.set_stress_game_callback([](Game &game, int random) {
        for (int i = 0; i < random; ++i) {
            game.alter_game_state();
        }
        return ONE_ERROR_NONE;
    });

    stress.set_stress_agent_callback(
        [](Agent &agent, int random) { return agent.send_soft_stop(random); });

    std::thread t1(run, &stress, seconds(60), milliseconds(0));
    std::thread t2(run, &stress, seconds(60), milliseconds(0));
    std::thread t3(run, &stress, seconds(60), milliseconds(0));

    t1.join();
    t2.join();
    t3.join();

    auto &agent = stress.agent();
    auto &game = stress.game();

    REQUIRE(0 < game.soft_stop_receive_count());
    REQUIRE(0 < agent.live_state_receive_count());

    L_INFO("soak:test high usage over a long period on multiple threads");
    L_INFO("Agent received: " + std::to_string(agent.live_state_receive_count()) +
           " live_state messages");
    L_INFO("Game received: " + std::to_string(game.soft_stop_receive_count()) +
           " soft_stop messages");

    stress.log_agent_error_tally();
    stress.log_game_error_tally();
    REQUIRE(true);
}

TEST_CASE("soak:test very high usage over a very long period on multiple threads",
          "[stress]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 18000;
    StressHarness stress(address, port);

    stress.set_stress_game_callback([](Game &game, int random) {
        for (int i = 0; i < random; ++i) {
            game.alter_game_state();
        }
        return ONE_ERROR_NONE;
    });

    stress.set_stress_agent_callback([](Agent &agent, int random) {
        for (int i = 0; i < random; ++i) {
            auto err = agent.send_soft_stop(random);
            if (is_error(err)) {
                return err;
            }
        }

        return ONE_ERROR_NONE;
    });

    std::thread t1(run, &stress, seconds(600), milliseconds(0));
    std::thread t2(run, &stress, seconds(600), milliseconds(0));
    std::thread t3(run, &stress, seconds(600), milliseconds(0));
    std::thread t4(run, &stress, seconds(600), milliseconds(0));
    std::thread t5(run, &stress, seconds(600), milliseconds(0));

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    auto &agent = stress.agent();
    auto &game = stress.game();

    REQUIRE(0 < game.soft_stop_receive_count());
    REQUIRE(0 < agent.live_state_receive_count());

    L_INFO("soak:test very high usage over a very long period on multiple threads");
    L_INFO("Agent received: " + std::to_string(agent.live_state_receive_count()) +
           " live_state messages");
    L_INFO("Game received: " + std::to_string(game.soft_stop_receive_count()) +
           " soft_stop messages");

    stress.log_agent_error_tally();
    stress.log_game_error_tally();
    REQUIRE(true);
}

TEST_CASE("soak-days:test normal load over many days", "[stress]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 18000;
    StressHarness stress(address, port);
    stress.set_game_callback_probability(5);
    stress.set_agent_callback_probability(5);
    stress.set_game_update_probability(100);
    stress.set_agent_update_probability(100);

    stress.set_stress_game_callback([](Game &game, int) {
        game.alter_game_state();
        return ONE_ERROR_NONE;
    });

    stress.set_stress_agent_callback(
        [](Agent &agent, int random) { return agent.send_soft_stop(random); });

    const seconds day = seconds(86400);
    stress.run(2 * day, milliseconds(200));

    auto &agent = stress.agent();
    auto &game = stress.game();

    REQUIRE(0 < game.soft_stop_receive_count());
    REQUIRE(0 < agent.live_state_receive_count());

    L_INFO("soak-days:test normal load over many days");
    L_INFO("Agent received: " + std::to_string(agent.live_state_receive_count()) +
           " live_state messages");
    L_INFO("Game received: " + std::to_string(game.soft_stop_receive_count()) +
           " soft_stop messages");

    stress.log_agent_error_tally();
    stress.log_game_error_tally();
    REQUIRE(true);
}
