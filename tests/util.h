#pragma once

#include <one/agent/agent.h>
#include <one/game/game.h>

#include <functional>

namespace i3d {
namespace one {

void sleep(int ms);

// Loops for given number of times, calling the given function. If the function
// returns true, then the loop breaks. If the function returns false, then
// the loop sleeps ms_per_loop milliseconds and then continues.
void for_sleep(int count, int ms_per_loop, std::function<bool()> cb);

// Waits until either the timeout has elapsed, or the given function returns
// true.
//
// The given function is called repeatedly at small time intervals. The interval
// is near 1 millisecond but system dependent. On most systems should be in the
// single millisecond range or less, but can be as high as tens of milliseconds
// on some systems.
//
// Returns true if done returned true, false if the timeout was reached.
bool wait_until(int timeout_ms, std::function<bool()> check);

// Loops for given number of times, calling the Agent & game update functions.
void pump_updates(int count, int ms_per_loop, Agent &agent, game::Game &game);

}  // namespace one
}  // namespace i3d
