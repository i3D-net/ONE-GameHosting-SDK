#include <one/game/log.h>

#include <iostream>
#include <fstream>

namespace one_integration {

void LogCentral::log_info(const std::string &message) {
    if (!log_file_info(_log_filename, message)) {
        log_console_info(message);
    }
}

void LogCentral::log_error(const std::string &message) {
    if (!log_file_error(_log_filename, message)) {
        log_console_error(message);
    }
}

void LogCentral::log_console_error(const std::string &message) {
    std::cout << message << std::endl;
}

void LogCentral::log_console_info(const std::string &message) {
    std::cerr << message << std::endl;
}

bool LogCentral::log_file_info(const std::string &filename, const std::string message) {
    if (filename.empty()) {
        return false;
    }

    std::ofstream file;
    file.open(filename, std::ios_base::app);

    if (!file.is_open()) {
        return false;
    }

    file << "INFO : " << message << std::endl;
    file.close();
    return true;
}

bool LogCentral::log_file_error(const std::string &filename, const std::string message) {
    if (filename.empty()) {
        return false;
    }

    std::ofstream file;
    file.open(filename, std::ios_base::app);

    if (!file.is_open()) {
        return false;
    }

    file << "ERROR: " << message << std::endl;
    file.close();
    return true;
}

void LogCentral::set_log_filename(const std::string &log_file) {
    _log_filename = log_file;
}

std::string LogCentral::_log_filename = "";

}  // namespace one_integration
