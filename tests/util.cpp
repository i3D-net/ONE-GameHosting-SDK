#include <util.h>

#include <chrono>
#include <thread>

namespace one {

void sleep(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void for_sleep(int count, int ms_per_loop, std::function<bool()> cb) {
    for (int i = 0; i < count; i++) {
        if (cb()) {
            break;
        }
        sleep(ms_per_loop);
    }
}

}  // namespace one
