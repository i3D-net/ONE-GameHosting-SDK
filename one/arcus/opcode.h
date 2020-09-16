#pragma once

#include <one/arcus/internal/version.h>

namespace i3d {
namespace one {

// Todo - set final opcode values.
enum class Opcode {
    invalid = 0,
    hello,  // Used internally by the connection.
    ping,   // Used internally by the connection.
    soft_stop_request,
    allocated_request,
    meta_data_request,
    live_state_request,
    live_state_response,
    player_joined_event_response,
    player_left_response,
    host_information_request,
    host_information_response,
    application_instance_information_request,
    application_instance_information_response,
    application_instance_get_status_request,
    application_instance_get_status_response,
    application_instance_set_status_request,
    application_instance_set_status_response,
};

// To finalize when the list of supported opcode is confirmed.
constexpr bool is_opcode_supported_v2(Opcode code) {
    return code == Opcode::hello || code == Opcode::ping ||
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
