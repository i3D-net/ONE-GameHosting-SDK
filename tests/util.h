#pragma once

#include <functional>

namespace one {

void sleep(int ms);

// Loops for given number of times, calling the given function. If the function
// returns true, then the loop breaks. If the function returns false, then
// the loop sleeps ms_per_loop milliseconds and then continues.
void for_sleep(int count, int ms_per_loop, std::function<bool()> cb);

}  // namespace one
