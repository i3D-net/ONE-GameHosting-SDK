#include <tests/harness.h>
#include <tests/util.h>

#include <one/game/log.h>

namespace i3d {
namespace one {

Harness::Harness(const char *address, unsigned int port)
    : _before_game_update_callback(nullptr)
    , _before_agent_update_callback(nullptr)
    , _game_error_tally{0}
    , _agent_error_tally{0}
    , _rd()
    , _re(_rd())
    , _uniform_dist(0, 100) {
    _game.init(port, 16, "test name", "test map", "test mode", "test version",
               seconds(0));
    _game.set_quiet(true);
    _agent.init(address, port);
    _agent.set_quiet(true);
    set_game_callback_probability(100);
    set_agent_callback_probability(100);
    set_game_update_probability(100);
    set_agent_update_probability(100);
}

Harness::~Harness() {
    _game.shutdown();
}

void Harness::set_game_callback_probability(const int probability) {
    if (!set_probability(probability, _game_callback_probability)) {
        L_ERROR("failed to set game callback probability");
    }
}

void Harness::set_agent_callback_probability(const int probability) {
    if (!set_probability(probability, _agent_callback_probability)) {
        L_ERROR("failed to set agent callback probability");
    }
}

void Harness::set_game_update_probability(const int probability) {
    if (!set_probability(probability, _game_update_probability)) {
        L_ERROR("failed to set game update probability");
    }
}

void Harness::set_agent_update_probability(const int probability) {
    if (!set_probability(probability, _agent_update_probability)) {
        L_ERROR("failed to set agent update probability");
    }
}

void Harness::set_before_game_update_callback(
    std::function<Error(one_integration::Game &, int)> callback) {
    _before_game_update_callback = callback;
}

void Harness::set_before_agent_update_callback(
    std::function<Error(Agent &, int)> callback) {
    _before_agent_update_callback = callback;
}

void Harness::run(seconds duration, milliseconds sleep) {
    for_sleep_duration(duration, sleep, [&]() {
        const std::lock_guard<std::mutex> lock(_run);

        if (_game.one_server_wrapper().status() ==
            one_integration::OneServerWrapper::Status::ready) {
            if (_before_game_update_callback != nullptr && game_need_callback()) {
                tally_game_error(
                    _before_game_update_callback(_game, random_percentage()));
            }

            if (_before_agent_update_callback != nullptr && agent_need_callback()) {
                tally_agent_error(
                    _before_agent_update_callback(_agent, random_percentage()));
            }
        }

        if (game_need_update()) {
            _game.update();
        }

        if (agent_need_update()) {
            tally_agent_error(_agent.update());
        }

        return false;
    });
}

void Harness::log_game_error_tally() const {
    L_INFO("The current harness as the current game error tally:");
    log_error_tally(_game_error_tally);
}

void Harness::log_agent_error_tally() const {
    L_INFO("The current harness as the current agent error tally:");
    log_error_tally(_agent_error_tally);
}

int Harness::random_percentage() {
    const std::lock_guard<std::mutex> lock(_harness);
    return _uniform_dist(_re);
}

void Harness::tally_game_error(Error e) {
    const std::lock_guard<std::mutex> lock(_harness);
    ++_game_error_tally[e];
}

void Harness::tally_agent_error(Error e) {
    const std::lock_guard<std::mutex> lock(_harness);
    ++_agent_error_tally[e];
}

void Harness::log_error_tally(const std::array<long, ONE_ERROR_COUNT> &tally) const {
    OneError err = ONE_ERROR_NONE;
    unsigned int total = 0;
    for (unsigned int i = 0; i < ONE_ERROR_COUNT; ++i) {
        err = static_cast<OneError>(i);
        total += tally[err];
    }

    if (total != 0) {
        L_INFO("\tONE_ERROR_NONE: " + std::to_string(tally[ONE_ERROR_NONE]) + " (" +
               std::to_string((static_cast<double>(tally[ONE_ERROR_NONE]) /
                               static_cast<double>(total)) *
                              100.0) +
               " %)");
    } else {
        L_INFO("\tONE_ERROR_NONE: " + std::to_string(tally[ONE_ERROR_NONE]));
    }

    for (unsigned int i = 1; i < ONE_ERROR_COUNT; ++i) {
        err = static_cast<OneError>(i);
        if (tally[err] == 0) {
            continue;
        }

        if (total != 0) {
            L_INFO("\t" + std::string(error_text(err)) + ": " +
                   std::to_string(tally[err]) + " (" +
                   std::to_string(
                       (static_cast<double>(tally[err]) / static_cast<double>(total)) *
                       100.0) +
                   " %)");
        } else {
            L_INFO("\t" + std::string(error_text(err)) + ": " +
                   std::to_string(tally[err]));
        }
    }
}

bool Harness::set_probability(int probability, int &output) {
    if (probability < 0) {
        return false;
    }

    if (100 < probability) {
        return false;
    }

    output = probability;
    return true;
}

bool Harness::game_need_callback() {
    if (_game_callback_probability < random_percentage()) {
        return false;
    }

    return true;
}

bool Harness::agent_need_callback() {
    if (_agent_callback_probability < random_percentage()) {
        return false;
    }

    return true;
}

bool Harness::game_need_update() {
    if (_game_update_probability < random_percentage()) {
        return false;
    }

    return true;
}

bool Harness::agent_need_update() {
    if (_agent_update_probability < random_percentage()) {
        return false;
    }

    return true;
}

StressHarness::StressHarness(const char *address, unsigned int port)
    : Harness(address, port) {}

void StressHarness::set_stress_game_callback(
    std::function<Error(one_integration::Game &, int)> callback) {
    _stress_game_callback = callback;
    set_before_game_update_callback(_stress_game_callback);
}

void StressHarness::set_stress_agent_callback(
    std::function<Error(Agent &, int)> callback) {
    _stress_agent_callback = callback;
    set_before_agent_update_callback(_stress_agent_callback);
}

ChaosHarness::ChaosHarness(const char *address, unsigned int port)
    : Harness(address, port) {
    set_before_game_update_callback(_random_game_callback);
    set_before_agent_update_callback(_random_agent_callback);
}

void ChaosHarness::set_random_game_callback(
    std::function<Error(one_integration::Game &, int)> callback) {
    _random_game_callback = callback;
    set_before_game_update_callback(_random_game_callback);
}

void ChaosHarness::set_random_agent_callback(
    std::function<Error(Agent &, int)> callback) {
    _random_agent_callback = callback;
    set_before_agent_update_callback(_random_agent_callback);
}

}  // namespace one
}  // namespace i3d
