#include <util.h>

#include <one/game/one_server_wrapper.h>

#include <chrono>
#include <thread>

using namespace std::chrono;

namespace i3d {
namespace one {

void sleep(int ms) {
    std::this_thread::sleep_for(milliseconds(ms));
}

void for_sleep(int count, int ms_per_loop, std::function<bool()> cb) {
    for (int i = 0; i < count; i++) {
        if (cb()) {
            break;
        }
        sleep(ms_per_loop);
    }
}

bool wait_until(int timeout_ms, std::function<bool()> check) {
    const auto start = steady_clock::now();
    if (check()) {
        return true;
    }
    while (duration_cast<milliseconds>(steady_clock::now() - start).count() <
           timeout_ms) {
        std::this_thread::sleep_for(milliseconds(1));
        if (check()) {
            return true;
        }
    }
    return false;
}

void pump_updates(int count, int ms_per_loop, Agent &agent, one_integration::Game &game) {
    for_sleep(count, ms_per_loop, [&]() {
        game.update();
        agent.update();
        if (agent.client().status() == Client::Status::ready &&
            game.one_server_wrapper().status() ==
                one_integration::OneServerWrapper::Status::ready)
            return true;
        return false;
    });
}

}  // namespace one
}  // namespace i3d
