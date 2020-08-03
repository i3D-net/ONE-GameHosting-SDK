#include "codec.h"

#include <cstring>
#include <stdint.h>

namespace one {
namespace codec {

const Hello hello = Hello{{'a', 'r', 'c', 0}, (char)0x1, 0};  // namespace codec

bool validate_hello(const Hello &other) {
    auto a = (const char *)&hello;
    auto b = (const char *)&other;
    return std::strncmp(a, b, hello_size()) == 0;
}

const void *hello_data() { return &hello; }

// Header for regular Arcus messages.
struct Header {
    char flags;
    char opcode;
    char reserved[2];
    int64_t packet_id;
    uint64_t length;
};

int data_to_message(const void *data, size_t length, Message &message) {
    // handle byte order to a specific order for wire

    // 1. Reader static header struct
    // 2. validate
    // 3. convert payload, if any, from (char*, size_t) to one::Payload.
    // 4. Configure and return message.
    // 5. Error handling throughout.

    return -1;
}

int message_to_data(const Message &message, std::vector<char> &data) {
    // handle byte order to a specific order for wire

    // 1. Create output header struct.
    // 2. Set opcode.
    // 3. Serialize Payload to char*, size_t
    // 4. Error handling throughout.

    return -1;
}

}  // namespace codec
}  // namespace one