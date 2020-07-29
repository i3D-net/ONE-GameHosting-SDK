
#include <vector>

namespace one {

class Message;

namespace codec {

// The first packet, used for handshaking, is a hello packet.
struct Hello {
    char id[4];
    char version;
    char dummy[3];
};
// Todo: ensure equal alignment on all platforms.
// Should be equal since char is byte by standard, but needs confirmation.

// Returns true if the given bytes match the hello packet. Length must be equal
// to hello_size.
bool is_hello(const void *data, size_t length);

// Returns the data for the hello packet. Length is sizeof(Hello).
const void* hello_data();

// Convert byte data to a Message.
int data_to_message(const void *data, size_t length, Message &message);

// Convert a message to byte data.
int message_to_data(const Message &message, std::vector<char> &data);

} // namespace codec
} // namespace one

