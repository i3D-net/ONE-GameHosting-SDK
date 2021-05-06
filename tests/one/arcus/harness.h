#pragma once

#include <one/fake/agent/agent.h>
#include <one/arcus/error.h>
#include <one/fake/game/game.h>

#include <unordered_map>
#include <chrono>
#include <functional>
#include <mutex>
#include <random>

namespace i3d {
namespace one {

class Harness {
public:
    Harness(const char *address, unsigned int port);
    Harness(const Harness &) = delete;
    Harness &operator=(const Harness &) = delete;
    virtual ~Harness();

    // This will set the probability to invoke the callback for the game at each run loop.
    // A probability of 100 means the game will always be updated, a probability of 0
    // means the game will never be updated. The probability must be in the interval: [0,
    // 100].
    void set_game_callback_probability(const int probability);
    // This will set the probability to invoke the callback for the agent at each run
    // loop. A probability of 100 means the agent will always be updated, a probability of
    // 0 means the agent will never be updated. The probability must be in the interval:
    // [0, 100].
    void set_agent_callback_probability(const int probability);

    // This will set the probability to update the game at each run loop. A probability of
    // 100 means the game will always be updated, a probability of 0 means the game will
    // never be updated. The probability must be in the interval: [0, 100].
    void set_game_update_probability(const int probability);
    // This will set the probability to update the agent at each run loop. A probability
    // of 100 means the agent will always be updated, a probability of 0 means the agent
    // will never be updated. The probability must be in the interval: [0, 100].
    void set_agent_update_probability(const int probability);

    // Set the game update callback which is invoked before the update in each run loop
    // according to the update probability.
    void set_before_game_update_callback(
        std::function<OneError(one_integration::Game &game, int random)> callback);
    // Set the agent update callback which is invoked before the update in each run loop
    // according to the update probability.
    void set_before_agent_update_callback(
        std::function<OneError(Agent &agent, int random)> callback);

    // Loops for the given duration. At each loop is calls the before_update_callback,
    // game update and agent update.
    void run(std::chrono::seconds duration, std::chrono::milliseconds sleep);

    // Logs the breakdown of errors that occured by error type for the game.
    // It is usefull to get a high level view of the type of error that occured and their
    // frequency.
    void log_game_error_tally() const;

    // Logs the breakdown of errors that occured by error type for the agent.
    // It is usefull to get a high level view of the type of error that occured and their
    // frequency.
    void log_agent_error_tally() const;

    // Exposed to expose the message counts on the game for tests validation.
    const one_integration::Game &game() const {
        return _game;
    }

    // Exposed to expose the message counts on the agent for tests validation.
    const Agent &agent() const {
        return _agent;
    }

protected:
    // Returns a random number in the interval: [0, 100].
    int random_percentage();

    // Register an error in the game tally.
    void tally_game_error(OneError e);

    // Register an error in the agent tally.
    void tally_agent_error(OneError e);

    // Display the error tally. It is always showing the `ONE_ERROR_NONE` count. All other
    // error types are only shown if there count is greater than 0 to avoid spamming.
    void log_error_tally(const std::unordered_map<OneError, size_t> &tally) const;

    bool set_probability(int probability, int &output);

private:
    bool game_need_callback();
    bool agent_need_callback();

    bool game_need_update();
    bool agent_need_update();

    int _game_callback_probability;
    int _agent_callback_probability;

    int _game_update_probability;
    int _agent_update_probability;

    one_integration::Game _game;
    Agent _agent;

    std::function<OneError(one_integration::Game &game, int random)>
        _before_game_update_callback;
    std::function<OneError(Agent &agent, int random)> _before_agent_update_callback;
    std::unordered_map<OneError, size_t> _game_error_tally;
    std::unordered_map<OneError, size_t> _agent_error_tally;

    std::random_device _rd;
    std::default_random_engine _re;
    std::uniform_int_distribution<int> _uniform_dist;
    std::mutex _harness;
    std::mutex _run;
};

class StressHarness final : public Harness {
public:
    StressHarness(const char *address, unsigned int port);
    StressHarness(const StressHarness &) = delete;
    StressHarness &operator=(const StressHarness &) = delete;
    virtual ~StressHarness() override = default;

    void set_stress_game_callback(
        std::function<OneError(one_integration::Game &, int percent)> callback);
    void set_stress_agent_callback(
        std::function<OneError(Agent &, int percent)> callback);

private:
    std::function<OneError(one_integration::Game &, int percent)> _stress_game_callback;
    std::function<OneError(Agent &, int percent)> _stress_agent_callback;
};

class ChaosHarness final : public Harness {
public:
    ChaosHarness(const char *address, unsigned int port);
    ChaosHarness(const ChaosHarness &) = delete;
    ChaosHarness &operator=(const ChaosHarness &) = delete;
    virtual ~ChaosHarness() override = default;

    void set_random_game_callback(
        std::function<OneError(one_integration::Game &, int percent)> callback);
    void set_random_agent_callback(
        std::function<OneError(Agent &, int percent)> callback);

private:
    std::function<OneError(one_integration::Game &, int percent)> _random_game_callback;
    std::function<OneError(Agent &, int percent)> _random_agent_callback;
};

}  // namespace one
}  // namespace i3d
