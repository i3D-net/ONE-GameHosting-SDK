#pragma once

#include <one/agent/agent.h>
#include <one/game/game.h>

#include <functional>

namespace i3d {
namespace one {

// On some systems like windows, this may need to be called to get sleep calls
// to yield more quickly if millisecond periods are desired.
void start_high_resolution_sleep();
void end_high_resolution_sleep();

// Simple utitlity class that can be declared at a scope level to enable and
// disable high resolution sleep.
class ScopedHighResolutionSleepEnabler final {
public:
    ScopedHighResolutionSleepEnabler() { start_high_resolution_sleep(); }
    ~ScopedHighResolutionSleepEnabler() { end_high_resolution_sleep(); }
};

// This will start and end high resolution sleep.
void sleep(int ms);

// Loops for given number of times, calling the given function. If the function
// returns true, then the loop breaks. If the function returns false, then
// the loop sleeps ms_per_loop milliseconds and then continues for the duration is
// seconds.
void for_sleep_duration(std::chrono::seconds duration_seconds,
                        std::chrono::milliseconds ms_per_loop, std::function<bool()> cb);

// Loops for given number of times, calling the given function. If the function
// returns true, then the loop breaks. If the function returns false, then
// the loop sleeps ms_per_loop milliseconds and then continues.
// This will start and end high resolution sleep.
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
// This will start and end high resolution sleep.
bool wait_until(int timeout_ms, std::function<bool()> check);

// Loops for given number of times, calling the Agent and game update functions.
// This will start and end high resolution sleep.
void pump_updates(int count, int ms_per_loop, Agent &agent, one_integration::Game &game);

}  // namespace one
}  // namespace i3d
