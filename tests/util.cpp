#include <iostream>  // eldebug
#include <util.h>
#include <chrono>
#include <thread>
using namespace std::chrono;
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
            std::cout << "here\n";
            return false;
        }
    }
    std::cout << "there\n";
    return true;
}

}  // namespace one
