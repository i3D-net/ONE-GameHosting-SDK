#include <one/game/log.h>

#include <iostream>

namespace one_integration {

void log_info(const std::string &message) {
    std::cout << message << std::endl;
}

void log_error(const std::string &message) {
    std::cerr << message << std::endl;
}

}  // namespace one_integration
