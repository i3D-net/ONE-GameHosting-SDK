#include <one/arcus/internal/version.h>

namespace one {

// Todo - set final opcode values.
enum class Opcodes {
    hello = 0,
    soft_stop = 1,
    allocated = 2,
    unsuported_v0,
    unsuported_v1,
    unknown
};

constexpr bool is_opcode_supported_v0(Opcodes code) {
    switch (code) {
        case Opcodes::hello:
        case Opcodes::soft_stop:
        case Opcodes::allocated:
            return true;
        case Opcodes::unsuported_v0:
        case Opcodes::unknown:
        default:
            return false;
    }
}

// To finalize when the list of supported opcode is confirmed.
constexpr bool is_opcode_supported_v1(Opcodes code) {
    switch (code) {
        case Opcodes::hello:
        case Opcodes::soft_stop:
        case Opcodes::allocated:
        case Opcodes::unsuported_v0:
            return true;
        case Opcodes::unsuported_v1:
        case Opcodes::unknown:
        default:
            return false;
    }
}

constexpr bool is_opcode_supported(Opcodes code) {
    if (arcus_selector<arcus_protocol::current_version()>::is_version_supported(ArcusVersion::V0)) {
        return is_opcode_supported_v0(code);
    }

    if (arcus_selector<arcus_protocol::current_version()>::is_version_supported(ArcusVersion::V1)) {
        return is_opcode_supported_v1(code);
    }

    return false;
}

}  // namespace one
