#include "codec.h"

namespace one {
namespace codec {

bool is_hello(const void *data, size_t length) {
    if (length != sizeof(Hello)) {
        return false;
    }

    // handle byte order to a specific order for wire
    // cast to Hello
    // check each field...
    // 1. detect static Arcus protocol prefix 4-byte "ARC\0"
    // 2. ...
    return true;
}

const void* hello_data() {
    // Todo: ensure same byte order in result pointer...
    static const Hello hello{{'a', 'r', 'c', 0}, (char)0x1, 0};
    return &hello;
}

// Header for regular Arcus messages.
struct Header
{

};

int data_to_message(const void *data, size_t length, Message &message)
{
    // handle byte order to a specific order for wire

    // 1. Reader static header struct
    // 2. validate
    // 3. convert payload, if any, from (char*, size_t) to one::Payload.
    // 4. Configure and return message.
    // 5. Error handling throughout.

    return -1;
}


int message_to_data(const Message &message, std::vector<char> &data)
{
    // handle byte order to a specific order for wire

    // 1. Create output header struct.
    // 2. Set opcode.
    // 3. Serialize Payload to char*, size_t
    // 4. Error handling throughout.

    return -1;
}

} // namespace codec
} // namespace one