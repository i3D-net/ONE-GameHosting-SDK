#pragma once

#include <string>

#define L_INFO(message) one_integration::log_info(message)
#define L_ERROR(message) one_integration::log_error(message)

namespace one_integration {

void log_info(const std::string &message);
void log_error(const std::string &message);

}  // namespace one_integration
