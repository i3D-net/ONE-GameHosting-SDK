
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
static_assert(sizeof(Hello) == 8, "hello struct alignment");
// Todo: ensure equal alignment on all platforms.
// Should be equal since char is byte by standard, but needs confirmation.

inline size_t hello_size() { return sizeof(Hello); }

bool validate_hello(const Hello &hello);

// Returns the data for the hello packet. Length is hello_size().
const void *hello_data();

// Convert byte data to a Message.
int data_to_message(const void *data, size_t length, Message &message);

// Convert a message to byte data.
int message_to_data(const Message &message, std::vector<char> &data);

}  // namespace codec
}  // namespace one
