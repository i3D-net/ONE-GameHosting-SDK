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

bool wait_with_cancel(int wait_ms, std::function<bool()> cb) {
    const auto start = steady_clock::now();
    if (cb()) {
        return false;
    }
    while (duration_cast<milliseconds>(steady_clock::now() - start).count() > wait_ms) {
        std::this_thread::sleep_for(milliseconds(1));
        if (cb()) {
            return false;
        }
    }
    return true;
}

void pump_updates(int count, int ms_per_loop, Agent &agent, game::Game &game) {
    for_sleep(count, ms_per_loop, [&]() {
        game.update();
        agent.update();
        if (agent.client().status() == Client::Status::ready &&
            game.one_server_wrapper().status() == game::OneServerWrapper::Status::ready)
            return true;
        return false;
    });
}

}  // namespace one
}  // namespace i3d
