#include <one/ping/types.h>

namespace i3d {
namespace ping {

std::string to_std_string(const String &str) {
    return std::string(str.c_str());
}

String to_one_string(const std::string &str) {
    return String(str.c_str());
}

}  // namespace ping
}  // namespace i3d