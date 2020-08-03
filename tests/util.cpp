#include "util.h"

#include <chrono>
#include <thread>

namespace one {

void sleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

} // namespace one