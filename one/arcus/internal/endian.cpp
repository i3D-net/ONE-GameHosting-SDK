#include "endian.h"

namespace one {
namespace endian {

Arch which() {
    union _ {
        int int_val;
        char char_val[sizeof(int)];
    } u;

    u.int_val = 1;
    return (u.char_val[sizeof(int) - 1] == 1) ? Arch::big : Arch::little;
}

uint32_t swap_uint32(uint32_t val) {
    uint32_t tmp = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (tmp << 16) | (tmp >> 16);
}

}  // namespace endian
}  // namespace one
