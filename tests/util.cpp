#include <util.h>

#include <chrono>
#include <thread>

#include <one/arcus/c_platform.h>

#ifdef ONE_WINDOWS
    #include <Windows.h>
#endif

#include <one/game/one_server_wrapper.h>

using namespace std::chrono;

namespace i3d {
namespace one {

void start_high_resolution_sleep() {
#ifdef ONE_WINDOWS
    timeBeginPeriod(1);
#endif
}

void end_high_resolution_sleep() {
#ifdef ONE_WINDOWS
    timeEndPeriod(1);
#endif
}

void sleep(int ms) {
    std::this_thread::sleep_for(milliseconds(ms));
}

void for_sleep_duration(std::chrono::seconds duration_seconds,
                        std::chrono::milliseconds ms_per_loop, std::function<bool()> cb) {
    const auto begin = std::chrono::steady_clock::now();

    while (true) {
        if (cb()) {
            break;
        }

        sleep(ms_per_loop.count());
        const auto current = std::chrono::steady_clock::now();

        if (duration_seconds <
            std::chrono::duration_cast<std::chrono::seconds>(current - begin)) {
            break;
        }
    }
}

void for_sleep(int count, int ms_per_loop, std::function<bool()> cb) {
    ScopedHighResolutionSleepEnabler enabler;

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

    ScopedHighResolutionSleepEnabler enabler;
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
