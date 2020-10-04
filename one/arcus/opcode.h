#pragma once

#include <one/arcus/internal/version.h>

namespace i3d {
namespace one {

enum class Opcode {
    invalid = 0,
    health = 0x02,  // Used internally by the connection.
    hello = 0x04,   // Used internally by the connection.
    soft_stop = 0x30,
    allocated = 0x60,
    metadata = 0x40,
    live_state_response = 0x21,
    host_information_response = 0x51,
    application_instance_information_response = 0x71,
    application_instance_set_status_request = 0x74,
};

// To finalize when the list of supported opcode is confirmed.
constexpr bool is_opcode_supported_v2(Opcode code) {
    return code == Opcode::health || code == Opcode::hello || code == Opcode::soft_stop ||
           code == Opcode::allocated || code == Opcode::metadata ||
           code == Opcode::live_state_response ||
           code == Opcode::host_information_response ||
           code == Opcode::application_instance_information_response ||
           code == Opcode::application_instance_set_status_request;
}

inline bool is_opcode_supported(Opcode code) {
    if (arcus_selector<arcus_protocol::current_version()>::is_version_supported(
            ArcusVersion::V2)) {
        return is_opcode_supported_v2(code);
    }

    return false;
}

}  // namespace one
}  // namespace i3d
