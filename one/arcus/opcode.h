#pragma once

#include <one/arcus/internal/version.h>

namespace i3d {
namespace one {

enum class Opcode {
    invalid = 0,
    Health = 0x02,  // Used internally by the connection.
    hello = 0x04,   // Used internally by the connection.
    soft_stop_request = 0x30,
    allocated_request = 0x60,
    meta_data_request = 0x40,
    live_state_request = 0x20,
    live_state_response = 0x21,
    player_joined_event_response = 0x22,
    player_left_response = 0x23,
    host_information_request = 0x50,
    host_information_response = 0x51,
    application_instance_information_request = 0x70,
    application_instance_information_response = 0x71,
    application_instance_get_status_request = 0x72,
    application_instance_get_status_response = 0x73,
    application_instance_set_status_request = 0x74,
    application_instance_set_status_response = 0x75,
};

// To finalize when the list of supported opcode is confirmed.
constexpr bool is_opcode_supported_v2(Opcode code) {
    return code == Opcode::Health || code == Opcode::hello ||
           code == Opcode::soft_stop_request || code == Opcode::allocated_request ||
           code == Opcode::meta_data_request || code == Opcode::live_state_request ||
           code == Opcode::live_state_response ||
           code == Opcode::player_joined_event_response ||
           code == Opcode::player_left_response ||
           code == Opcode::host_information_request ||
           code == Opcode::host_information_response ||
           code == Opcode::application_instance_information_request ||
           code == Opcode::application_instance_information_response ||
           code == Opcode::application_instance_get_status_request ||
           code == Opcode::application_instance_get_status_response ||
           code == Opcode::application_instance_set_status_request ||
           code == Opcode::application_instance_set_status_response;
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
