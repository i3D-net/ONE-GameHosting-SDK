#include <one/agent/log.h>

#include <iostream>

namespace i3d {
namespace one {

void log_info(const String &message) {
    std::cout << message << std::endl;
}

void log_error(const String &message) {
    std::cerr << message << std::endl;
}

}
}
