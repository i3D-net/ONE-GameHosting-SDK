#include <one/fake/arcus/agent/log.h>

#include <ctime>
#include <iostream>

namespace i3d {
namespace one {

namespace {

std::string timestamp() {
    time_t now;
    time(&now);
    char buf[sizeof "2020-10-29T10:17:00Z"];
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&now));
    return std::string(buf);
}

}  // namespace

void log_info(const String &message) {
    std::cout << timestamp() << " AGENT INFO : " << message << std::endl;
}

void log_error(const String &message) {
    std::cout << timestamp() << " AGENT ERROR: " << message << std::endl;
}

}  // namespace one
}  // namespace i3d
