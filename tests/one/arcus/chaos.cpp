#include <catch.hpp>
#include <tests/one/arcus/util.h>

#include <thread>

#include <tests/one/arcus/harness.h>
#include <one/fake/game/log.h>

using namespace one_integration;
using namespace i3d::one;
using namespace std::chrono;

void run(ChaosHarness *harness, seconds duration, milliseconds sleep) {
    harness->run(duration, sleep);
}

TEST_CASE("soak:random update and callback", "[choas]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 18100;
    ChaosHarness chaos(address, port);
    chaos.set_game_callback_probability(80);
    chaos.set_agent_callback_probability(80);
    chaos.set_game_update_probability(80);
    chaos.set_agent_update_probability(80);

    chaos.set_random_game_callback([](Game &game, int) {
        game.alter_game_state();
        return ONE_ERROR_NONE;
    });

    chaos.set_random_agent_callback(
        [](Agent &agent, int random) { return agent.send_soft_stop(random); });

    chaos.run(seconds(60), milliseconds(50));

    auto &agent = chaos.agent();
    auto &game = chaos.game();

    REQUIRE(0 < game.soft_stop_receive_count());
    REQUIRE(0 < agent.live_state_receive_count());

    L_INFO("soak:random update and callback");
    L_INFO("Agent received: " + std::to_string(agent.live_state_receive_count()) +
           " live_state messages");
    L_INFO("Game received: " + std::to_string(game.soft_stop_receive_count()) +
           " soft_stop messages");

    chaos.log_agent_error_tally();
    chaos.log_game_error_tally();
    REQUIRE(true);
}

TEST_CASE("soak:random update and callback low frequency update", "[choas]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 18100;
    ChaosHarness chaos(address, port);
    chaos.set_game_callback_probability(50);
    chaos.set_agent_callback_probability(50);
    chaos.set_game_update_probability(50);
    chaos.set_agent_update_probability(50);

    chaos.set_random_game_callback([](Game &game, int) {
        game.alter_game_state();
        return ONE_ERROR_NONE;
    });

    chaos.set_random_agent_callback(
        [](Agent &agent, int random) { return agent.send_soft_stop(random); });

    chaos.run(seconds(60), milliseconds(100));

    auto &agent = chaos.agent();
    auto &game = chaos.game();

    REQUIRE(0 < game.soft_stop_receive_count());
    REQUIRE(0 < agent.live_state_receive_count());

    L_INFO("soak:random update and callback low frequency update");
    L_INFO("Agent received: " + std::to_string(agent.live_state_receive_count()) +
           " live_state messages");
    L_INFO("Game received: " + std::to_string(game.soft_stop_receive_count()) +
           " soft_stop messages");

    chaos.log_agent_error_tally();
    chaos.log_game_error_tally();
    REQUIRE(true);
}

TEST_CASE("soak:random update and callback low frequency update on multiple threads",
          "[choas]") {
    const auto address = "127.0.0.1";
    const unsigned int port = 18100;
    ChaosHarness chaos(address, port);
    chaos.set_game_callback_probability(50);
    chaos.set_agent_callback_probability(50);
    chaos.set_game_update_probability(50);
    chaos.set_agent_update_probability(50);

    chaos.set_random_game_callback([](Game &game, int) {
        game.alter_game_state();
        return ONE_ERROR_NONE;
    });

    chaos.set_random_agent_callback(
        [](Agent &agent, int random) { return agent.send_soft_stop(random); });

    std::thread t1(run, &chaos, seconds(60), milliseconds(100));
    std::thread t2(run, &chaos, seconds(60), milliseconds(100));
    std::thread t3(run, &chaos, seconds(60), milliseconds(100));

    t1.join();
    t2.join();
    t3.join();

    auto &agent = chaos.agent();
    auto &game = chaos.game();

    REQUIRE(0 < game.soft_stop_receive_count());
    REQUIRE(0 < agent.live_state_receive_count());

    L_INFO("soak:random update and callback low frequency update on multiple threads");
    L_INFO("Agent received: " + std::to_string(agent.live_state_receive_count()) +
           " live_state messages");
    L_INFO("Game received: " + std::to_string(game.soft_stop_receive_count()) +
           " soft_stop messages");

    chaos.log_agent_error_tally();
    chaos.log_game_error_tally();
    REQUIRE(true);
}
