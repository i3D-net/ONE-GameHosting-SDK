
#include <vector>

namespace one {

class Message;

// The codec provides conversion to and from byte data for Arcus types.
namespace codec {

// The first packet, used for handshaking, is a hello packet.
struct Hello {
    char id[4];
    char version;
    char dummy[3];
};
static_assert(sizeof(Hello) == 8, "hello struct alignment");

inline size_t hello_size() { return sizeof(Hello); }

// Returns true if the given Hello packet matches what is expected by
// this version of the SDK.
bool validate_hello(const Hello &hello);

// Returns the valid, expected Hello values.
const Hello &valid_hello();

// Convert byte data to a Message.
int data_to_message(const void *data, size_t length, Message &message);

// Convert a message to byte data.
int message_to_data(const Message &message, std::vector<char> &data);

}  // namespace codec
}  // namespace one
