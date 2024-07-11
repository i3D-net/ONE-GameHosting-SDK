#pragma once

#include <chrono>

namespace i3d {
namespace one {

// IntervalTimer is used to easily track if an interval in seconds has expired.
class IntervalTimer final {
public:
    IntervalTimer(long long seconds);
    IntervalTimer() = delete;
    IntervalTimer(IntervalTimer &other) = delete;

    // Update internal timer to time now. Returns true if the delay interval passed.
    // If the interval has passed, then counting resets using the current time.
    // Will always return true and set the timer the first time it is called.
    bool update();

    // Synchronizes the timer with time now, starting a new interval.
    void sync_now();

private:
    long long _interval;
    std::chrono::steady_clock::time_point _last_trigger_time;
};

}  // namespace one
}  // namespace i3d