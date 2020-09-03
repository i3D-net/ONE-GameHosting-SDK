#pragma once

#include <string>

#define L_INFO(message) game::log_info(message)
#define L_ERROR(message) game::log_error(message)

namespace game {

void log_info(const std::string &message);
void log_error(const std::string &message);

}  // namespace game
