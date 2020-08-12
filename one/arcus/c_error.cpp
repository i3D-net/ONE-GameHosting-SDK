#include <one/arcus/c_error.h>

#include <unordered_map>
#include <string>

namespace one {
namespace {

#define ERROR_MAP_PAIR(e) e, #e
const char *error_text(OneError err) {
    static std::unordered_map<OneError, std::string> lookup = {
        {ERROR_MAP_PAIR(ONE_ERROR_NONE)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_RECEIVE_BEFORE_SEND)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_HELLO_SEND_FAILED)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_HELLO_RECEIVE_FAILED)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_HELLO_VERSION_MISMATCH)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_HELLO_TOO_BIG)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_HELLO_INVALID)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_HELLO_MESSAGE_SEND_FAILED)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_HELLO_MESSAGE_RECEIVE_FAILED)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_HELLO_MESSAGE_HEADER_TOO_BIG)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_HELLO_MESSAGE_REPLY_INVALID)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_UPDATE_AFTER_ERROR)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_UPDATE_READY_FAIL)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_SEND_FAIL)},
        {ERROR_MAP_PAIR(ONE_ERROR_CONNECTION_SEND_FAIL)}};
    auto it = lookup.find(err);
    if (it == lookup.end()) {
        return "";
    }
    return it->second.c_str();
}
}  // unnamed namespace

}  // namespace one

#ifdef __cplusplus
extern "C" {
#endif

const char *error_text(OneError err) {
    return one::error_text(err);
}

#ifdef __cplusplus
};
#endif
