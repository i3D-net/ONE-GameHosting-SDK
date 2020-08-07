#pragma once

#include <one/arcus/internal/version.h>

namespace one {

// Todo - set final opcode values.
enum class Opcodes { hello, soft_stop, allocated, live_state_request, live_state, invalid };

// To finalize when the list of supported opcode is confirmed.
constexpr bool is_opcode_supported_v2(Opcodes code) {
    return code == Opcodes::hello || code == Opcodes::soft_stop || code == Opcodes::allocated ||
           code == Opcodes::live_state_request || code == Opcodes::live_state;
}

inline bool is_opcode_supported(Opcodes code) {
    if (arcus_selector<arcus_protocol::current_version()>::is_version_supported(ArcusVersion::V2)) {
        return is_opcode_supported_v2(code);
    }

    return false;
}

}  // namespace one
