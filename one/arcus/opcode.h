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
    reverse_metadata = 0x41,
    live_state = 0x20,
    host_information = 0x50,
    application_instance_information = 0x70,
    application_instance_status = 0x71,
    custom_command = 0x45
};

// To finalize when the list of supported opcode is confirmed.
constexpr bool is_opcode_supported_v2(Opcode code) {
    return code == Opcode::health || code == Opcode::hello || code == Opcode::soft_stop ||
           code == Opcode::allocated || code == Opcode::metadata ||
           code == Opcode::reverse_metadata || code == Opcode::live_state ||
           code == Opcode::host_information ||
           code == Opcode::application_instance_information ||
           code == Opcode::application_instance_status || code == Opcode::custom_command;
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
