#include <one/arcus/internal/time.h>

namespace i3d {
namespace one {

IntervalTimer::IntervalTimer(long long seconds)
    : _interval(seconds), _last_trigger_time(std::chrono::steady_clock::duration::zero()) {}

bool IntervalTimer::update() {
    const auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - _last_trigger_time).count() >= _interval) {
        _last_trigger_time = now;
        return true;
    }
    return false;
}

void IntervalTimer::sync_now() {
    _last_trigger_time = std::chrono::steady_clock::now();
}

}  // namespace one
}  // namespace i3d