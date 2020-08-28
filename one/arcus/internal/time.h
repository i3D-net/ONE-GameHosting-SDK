#include <chrono>

using namespace std::chrono;

namespace one {

// IntervalTimer is used to easily track if an interval in seconds has expired.
class IntervalTimer final {
public:
    IntervalTimer(unsigned int seconds);
    IntervalTimer() = delete;
    IntervalTimer(IntervalTimer &other) = delete;

    // Update internal timer to time now. Returns true if the delay interval passed.
    // If the interval has passed, then counting resets using the current time.
    // Will always return true and set the timer the first time it is called.
    bool update();

    // Synchronizes the timer with time now, starting a new interval.
    void sync_now();

private:
    size_t _interval;
    steady_clock::time_point _last_trigger_time;
};

}  // namespace one