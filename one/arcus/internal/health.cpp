#include <one/arcus/internal/health.h>

#include <one/arcus/internal/socket.h>

namespace one {

HealthChecker::HealthChecker(size_t send_interval_seconds,
                             size_t receive_interval_seconds)
    : _send_timer(send_interval_seconds), _receive_timer(receive_interval_seconds) {}

Error HealthChecker::process_send(Socket &socket) {
    const bool should_send = _send_timer.update();
    if (!should_send) return ONE_ERROR_NONE;

    // send a health message.

    return ONE_ERROR_NONE;
}

void HealthChecker::reset_receive_timer() {
    _receive_timer.sync_now();
}

bool HealthChecker::process_receive() {
    return _receive_timer.update();
}

}  // namespace one