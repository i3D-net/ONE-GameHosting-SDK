#pragma once

#include <one/arcus/error.h>
#include <one/arcus/opcode.h>

#include <stdint.h>
#include <array>

namespace i3d {
namespace one {

class Message;
class Payload;

// The codec provides conversion to and from byte data for Arcus types.
namespace codec {

//-----------------
// Handshake Hello.

// The first packet, used for handshaking, is a hello packet.
struct Hello {
    char id[4];
    char version;
    char dummy;
};
static_assert(sizeof(Hello) == 6, "hello struct alignment");

constexpr size_t hello_size() {
    return sizeof(Hello);
}

// Returns true if the given Hello version is compatible with this version of the SDK.
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

constexpr size_t header_size() {
    return sizeof(Header);
}

constexpr size_t payload_max_size() {
    return (1024 * 128) - header_size();
}

// Ensuring that max size is smaller or equal to Stream buffers sizes of the connection's
// socket.
static_assert(sizeof(header_size() + payload_max_size()) <= 1024 * 128,
              "max header and payload size");

// Returns true if the given Header matches what is expected by
// this version of the SDK.
bool validate_header(const Header &header);

// Convert the first message from data from at most data_size bytes. The read_data_size
// will contain the number of byte read and be equal to: codec::header_size() +
// header.length. The read_data_size is at least codec::header_size() and at most
// codec::header_size() + codec::payload_max_size().
OneError data_to_message(const void *data, const size_t data_size, size_t &read_data_size,
                      Header &header, Message &message);

// Convert a Message to byte data.
OneError message_to_data(const uint32_t packet_id, const Message &message,
                      size_t &data_length,
                      std::array<char, header_size() + payload_max_size()> &data);

// Convert byte data to a Header. Length must be header_size().
OneError data_to_header(const void *data, size_t length, Header &header);

// Convert a Header to byte data.
OneError header_to_data(const Header &header, std::array<char, header_size()> &data);

// Convert byte data to a Payload. Length must be at most payload_max_size().
OneError data_to_payload(const void *data, size_t length, Payload &payload);

// Convert a Payload to byte data.
OneError payload_to_data(const Payload &payload, size_t &payload_length,
                      std::array<char, payload_max_size()> &data);

}  // namespace codec
}  // namespace one
}  // namespace i3d