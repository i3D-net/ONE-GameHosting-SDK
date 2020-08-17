#include <one/arcus/internal/codec.h>

#include <cstring>

namespace one {
namespace codec {

const Hello hello = Hello{{'a', 'r', 'c', 0}, (char)0x1, 0};  // namespace codec

bool validate_hello(const Hello &other) {
    const auto cmp = std::memcmp(&hello, &other, hello_size());
    return cmp == 0;
}

const Hello &valid_hello() {
    return hello;
}

bool validate_header(const Header &header) {
    // Minimal validation in the codec at the moment. Opcode will be handled
    // by message layer. Length will be handled by document reader.
    // Flags are not used at the moment and should be zero.
    return header.flags == (char)0x0;
}

int data_to_header(const void *data, size_t length, Header &header) {
    // handle byte order to a specific order for wire

    // 1. Reader static header struct
    // 2. validate
    // 3. convert payload, if any, from (char*, size_t) to one::Payload.
    // 4. Configure and return header.
    // 5. Error handling throughout.

    return -1;
}

void header_to_data(const Header &header, std::array<char, header_size()> &data) {
    // Todo: handle byte order to a specific order for wire

    std::memcpy(data.data(), &header, data.size());
}

}  // namespace codec
}  // namespace one
