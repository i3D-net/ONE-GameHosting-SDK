#include <one/arcus/internal/codec.h>

#include <one/arcus/internal/endian.h>
#include <one/arcus/internal/messages.h>
#include <one/arcus/message.h>

#include <cstring>
#include <algorithm>

namespace i3d {
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
    bool is_valid = true;
    is_valid &= header.flags == (char)0x0;
    is_valid &= is_opcode_supported(static_cast<Opcode>(header.opcode));
    return is_valid;
}

OneError data_to_message(const void *data, const size_t data_size, size_t &read_data_size,
                      Header &header, Message &message) {
    if (data_size < header_size()) {
        return ONE_ERROR_CODEC_DATA_LENGTH_TOO_SMALL_FOR_HEADER;
    }

    // See: https://en.cppreference.com/w/cpp/language/value_initialization
    // C++11 Value initialization
    header = Header{};
    auto err = data_to_header(data, header_size(), header);
    if (is_error(err)) return err;

    if (header.length > payload_max_size()) {
        return ONE_ERROR_CODEC_EXPECTED_DATA_LENGTH_TOO_BIG;
    }

    const size_t total_message_size = header_size() + header.length;
    if (data_size < total_message_size) {
        // The header has a payload length but the data isn't long enough -
        // cannot read the entire payload.
        return ONE_ERROR_CODEC_DATA_LENGTH_TOO_SMALL_FOR_PAYLOAD;
    }

    read_data_size = total_message_size;

    Payload payload;

    if (0 < header.length) {
        const size_t payload_length = header.length;
        const char *payload_data = static_cast<const char *>(data) + codec::header_size();
        err = payload.from_json({payload_data, payload_length});
        if (is_error(err)) return err;
    }

    const Opcode code = static_cast<Opcode>(header.opcode);
    err = message.init(code, payload);
    if (is_error(err)) {
        message.reset();
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError message_to_data(const uint32_t packet_id, const Message &message,
                      size_t &data_length,
                      std::array<char, header_size() + payload_max_size()> &data) {
    std::array<char, payload_max_size()> payload_data;
    size_t payload_length = 0;
    auto err = payload_to_data(message.payload(), payload_length, payload_data);
    if (is_error(err)) return err;

    // See: https://en.cppreference.com/w/cpp/language/value_initialization
    // C++11 Value initialization
    Header header{};
    header.opcode = static_cast<char>(message.code());
    header.packet_id = packet_id;
    header.length = payload_length;

    std::array<char, header_size()> header_data;
    err = header_to_data(header, header_data);
    if (is_error(err)) return err;

    data_length = header_size() + payload_length;
    std::copy(header_data.cbegin(), header_data.cend(), data.begin());

    if (0 < payload_length) {
        std::copy_n(payload_data.cbegin(), payload_length, data.begin() + header_size());
    }

    return ONE_ERROR_NONE;
}

OneError data_to_header(const void *data, size_t length, Header &header) {
    // handle byte order to a specific order for wire

    if (length < header_size()) {
        return ONE_ERROR_CODEC_HEADER_LENGTH_TOO_SMALL;
    }

    if (header_size() < length) {
        return ONE_ERROR_CODEC_HEADER_LENGTH_TOO_BIG;
    }

    std::memcpy(&header, data, length);

    // Endian handling. Network byte order for the Header itself is
    // non-standard: little.
    if (endian::which() == endian::Arch::little) {
        header.length = endian::swap_uint32(header.length);
        header.packet_id = endian::swap_uint32(header.packet_id);
    }

    if (!validate_header(header)) {
        return ONE_ERROR_CODEC_INVALID_HEADER;
    }

    return ONE_ERROR_NONE;
}

OneError header_to_data(const Header &header, std::array<char, header_size()> &data) {
    if (!validate_header(header)) {
        return ONE_ERROR_CODEC_INVALID_HEADER;
    }

    // Endian handling. Network byte order for the Header itself is
    // non-standard: little.
    Header swapped_header(header);
    if (endian::which() == endian::Arch::little) {
        swapped_header.length = endian::swap_uint32(header.length);
        swapped_header.packet_id = endian::swap_uint32(header.packet_id);
    }

    std::memcpy(data.data(), &swapped_header, header_size());
    return ONE_ERROR_NONE;
}

OneError data_to_payload(const void *data, size_t length, Payload &payload) {
    if (payload_max_size() < length) {
        return ONE_ERROR_CODEC_INVALID_MESSAGE_PAYLOAD_SIZE_TOO_BIG;
    }

    if (length == 0) {
        payload.clear();
        return ONE_ERROR_NONE;
    }

    auto err = payload.from_json({static_cast<const char *>(data), length});
    if (is_error(err)) return err;

    return ONE_ERROR_NONE;
}

OneError payload_to_data(const Payload &payload, size_t &payload_length,
                      std::array<char, payload_max_size()> &data) {
    if (payload.is_empty()) {
        payload_length = 0;
        return ONE_ERROR_NONE;
    }

    auto str = payload.to_json();
    payload_length = str.size();

    if (payload_max_size() < payload_length) {
        return ONE_ERROR_CODEC_INVALID_MESSAGE_PAYLOAD_SIZE_TOO_BIG;
    }

    std::memcpy(data.data(), str.c_str(), payload_length);
    return ONE_ERROR_NONE;
}

}  // namespace codec
}  // namespace one
}  // namespace i3d
