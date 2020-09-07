#include <cstdint>

namespace one {
namespace endian {

enum class Arch { big, little };

// Returns the architecture of the current device.
Arch which();

uint32_t swap_uint32(uint32_t value);

}  // namespace endian
}  // namespace one
