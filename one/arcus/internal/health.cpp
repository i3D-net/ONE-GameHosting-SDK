#include <one/arcus/internal/health.h>
#include <one/arcus/message.h>
#include <one/arcus/opcode.h>

namespace i3d {
namespace one {

HealthChecker::HealthChecker(size_t send_interval_seconds,
                             size_t receive_interval_seconds)
    : _send_timer(send_interval_seconds), _receive_timer(receive_interval_seconds) {
    // Sync the timer fresh so it doesn't fire immediately.
    _receive_timer.sync_now();
    _send_timer.sync_now();
}

Error HealthChecker::process_send(
    std::function<Error(std::function<Error(Message &)>)> sender) {
    const bool should_send = _send_timer.update();
    if (!should_send) return ONE_ERROR_NONE;

    _send_timer.sync_now();

    auto err = sender([](Message &message) {
        message.init(Opcode::Health, Payload());
        return ONE_ERROR_NONE;
    });
    return err;
}

void HealthChecker::reset_receive_timer() {
    _receive_timer.sync_now();
}

bool HealthChecker::process_receive() {
    return _receive_timer.update();
}

}  // namespace one
}  // namespace i3d