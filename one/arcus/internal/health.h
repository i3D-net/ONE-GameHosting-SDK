#pragma once

#include <functional>

#include <one/arcus/error.h>
#include <one/arcus/internal/time.h>

namespace i3d {
namespace one {

class Message;

// Health features:
// - sending of a health opcode message at an interval
// - tracking of a timer for use whenever any message is received, to alert
// when no message has been received for a period of time
class HealthChecker final {
public:
    // Optional defaults.
    static constexpr size_t health_check_send_interval_seconds = 5;
    static constexpr size_t health_check_receive_interval_seconds = 20;

    HealthChecker(size_t send_interval_seconds, size_t receive_interval_seconds);

    // Updates internal timer and sends a adds a health message if needed.
    OneError process_send(std::function<OneError(const Message &m)> sender);

    // Should be called whenever data is received from the client.
    void reset_receive_timer();

    // Returns true if the time since last call to reset_receive_timer exceeds
    // the health interval during which a message received from the client is
    // required. Resets the receive timer interval if true.
    bool process_receive();

private:
    HealthChecker() = delete;
    HealthChecker(HealthChecker &other) = delete;

    IntervalTimer _send_timer;     // To track when to send health messages.
    IntervalTimer _receive_timer;  // To track last message receipt time.
};

}  // namespace one
}  // namespace i3d
