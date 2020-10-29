#pragma once

#include <functional>

#include <one/arcus/types.h>

namespace i3d {
namespace one {

// Must match OneLogLevel in c_api.h.
enum class LogLevel { Info = 0, Error };

// Simple logger class that needs to be given the actual logging callback.
class Logger final {
public:
    Logger() : _logFn(nullptr) {}
    Logger(std::function<void(LogLevel, const String &)> logFn) : _logFn(logFn) {}

    void Log(LogLevel level, const String &message) const {
        if (_logFn == nullptr) return;
        _logFn(level, message);
    };

private:
    std::function<void(LogLevel, const String &)> _logFn;
};

}  // namespace one
}  // namespace i3d