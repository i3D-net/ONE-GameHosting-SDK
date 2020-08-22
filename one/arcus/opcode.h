#pragma once

#include <one/arcus/internal/version.h>

namespace one {

// Todo - set final opcode values.
enum class Opcode {
    invalid = 0,
    allocated_request,
    error_response,
    health,
    hello,
    host_information_request,
    live_state_request,
    live_state_response,
    meta_data_request,
    soft_stop_request
};

// To finalize when the list of supported opcode is confirmed.
constexpr bool is_opcode_supported_v2(Opcode code) {
    return code == Opcode::hello || code == Opcode::error_response ||
           code == Opcode::soft_stop_request || code == Opcode::allocated_request ||
           code == Opcode::meta_data_request || code == Opcode::live_state_request ||
           code == Opcode::live_state_response ||
           code == Opcode::host_information_request;
}

inline bool is_opcode_supported(Opcode code) {
    if (arcus_selector<arcus_protocol::current_version()>::is_version_supported(
            ArcusVersion::V2)) {
        return is_opcode_supported_v2(code);
    }

    return false;
}

}  // namespace one
