#pragma once

#include <stdint.h>
#include <vector>

namespace one {

class Message;

// The codec provides conversion to and from byte data for Arcus types.
namespace codec {

//-----------------
// Handshake Hello.

// The first packet, used for handshaking, is a hello packet.
struct Hello {
    char id[4];
    char version;
    char dummy[3];
};
static_assert(sizeof(Hello) == 8, "hello struct alignment");

inline size_t hello_size() {
    return sizeof(Hello);
}

// Returns true if the given Hello matches what is expected by
// this version of the SDK.
bool validate_hello(const Hello &hello);

// Returns the valid, expected Hello values.
const Hello &valid_hello();

//---------------
// Arcus Message.

// Header for regular Arcus messages.
struct Header {
    char flags;
    char opcode;
    char reserved[2];
    uint32_t packet_id;
    uint32_t length;
};
static_assert(sizeof(Header) == 12, "header struct alignment");

inline size_t header_size() {
    return sizeof(Header);
}

// Returns true if the given Header matches what is expected by
// this version of the SDK.
bool validate_header(const Header &header);

// Convert byte data to a Header. Length must be header_size().
int data_to_header(const void *data, size_t length, Header &header);

// Convert a Header to byte data.
int header_to_data(const Header &message, std::vector<char> &data);

}  // namespace codec
}  // namespace one
