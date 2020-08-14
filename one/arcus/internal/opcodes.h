#pragma once

#include <one/arcus/internal/version.h>

namespace one {

// Todo - set final opcode values.
enum class Opcodes {
    invalid = 0,
    hello,
    error_response,
    soft_stop_request,
    allocated_request,
    meta_data_request,
    live_state_request,
    live_state_response,
    host_information_request
};

// To finalize when the list of supported opcode is confirmed.
constexpr bool is_opcode_supported_v2(Opcodes code) {
    return code == Opcodes::hello || code == Opcodes::error_response ||
           code == Opcodes::soft_stop_request || code == Opcodes::allocated_request ||
           code == Opcodes::meta_data_request || code == Opcodes::live_state_request ||
           code == Opcodes::live_state_response || code == Opcodes::host_information_request;
}

inline bool is_opcode_supported(Opcodes code) {
    if (arcus_selector<arcus_protocol::current_version()>::is_version_supported(ArcusVersion::V2)) {
        return is_opcode_supported_v2(code);
    }

    return false;
}

}  // namespace one
