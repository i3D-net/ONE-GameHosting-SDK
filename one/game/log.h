#pragma once

#include <string>

#define L_INFO(message) one_integration::LogCentral::log_info(message)
#define L_ERROR(message) one_integration::LogCentral::log_error(message)

namespace one_integration {

class LogCentral {
public:
    // Try to log info in file _log_file first, if unable to log into a file it logs in
    // std::cout.
    static void log_info(const std::string &message);
    // Try to log info in file _log_file first, if unable to log into a file it logs in
    // std::cerr.
    static void log_error(const std::string &message);

    // Log message in std::cout.
    static void log_console_info(const std::string &message);
    // Log message in std::cerr.
    static void log_console_error(const std::string &message);

    // Log message into a file with `INFO :` prefix.
    static bool log_file_info(const std::string &filename, const std::string message);
    // Log message into a file with `ERROR:` prefix.
    static bool log_file_error(const std::string &filename, const std::string message);

    // Set the target log file used by `log_info` and `log_error`.
    static void set_log_filename(const std::string &log_filename);

private:
    static std::string timestamp();

    static std::string _log_filename;
};

}  // namespace one_integration
