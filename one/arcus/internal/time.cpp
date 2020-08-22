#include <one/arcus/internal/time.h>

namespace one {

IntervalTimer::IntervalTimer(unsigned int seconds)
    : _interval(seconds), _last_trigger_time(steady_clock::duration::zero()) {}

bool IntervalTimer::update() {
    const auto now = steady_clock::now();
    if (duration_cast<seconds>(now - _last_trigger_time).count() > _interval) {
        _last_trigger_time = now;
        return true;
    }
    return false;
}

void IntervalTimer::sync_now() {
    _last_trigger_time = steady_clock::now();
}

}  // namespace one