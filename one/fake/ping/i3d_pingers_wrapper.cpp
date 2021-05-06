#include <one/fake/ping/i3d_pingers_wrapper.h>

#include <assert.h>
#include <string>
#include <cstring>

#include <one/ping/c_api.h>
#include <one/ping/c_error.h>
#include <one/fake/ping/log.h>

namespace i3d_ping_integration {
namespace {

// Local cached memory function overrides, to assist in override pattern.
std::function<void *(size_t)> _alloc = nullptr;
std::function<void(void *)> _free = nullptr;
std::function<void *(void *, size_t)> _realloc = nullptr;

// Logger to pass into One.
void log(void *userdata, I3dPingLogLevel level, const char *message) {
    // userdata not used in this wrapper, but could point to instance of a logger
    // instead of a global logger.

    switch (level) {
        case I3D_PING_ERROR_LOG_LEVEL_INFO: {
            L_INFO(std::string("ONELOG: ") + message);
            break;
        }
        case I3D_PING_ERROR_LOG_LEVEL_ERROR: {
            L_ERROR(std::string("ONELOG: ") + message);
            break;
        }
        default: {
            L_ERROR(std::string("ONELOG: unknown log level: ") + message);
        }
    }
}

}  // namespace

I3dPingersWrapper::I3dPingersWrapper() : _pingers(nullptr) {}

I3dPingersWrapper::~I3dPingersWrapper() {
    shutdown();
}

bool I3dPingersWrapper::init(const AllocationHooks &hooks) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    if (_pingers != nullptr) {
        L_ERROR("already initialized");
        return false;
    }

    //----------------------
    // Set custom allocator.

    if (hooks.alloc && hooks.free && hooks.realloc) {
        // Cache off the overrides so that they can be called within the lambdas
        // because lambdas with captures may not be passed to the C API.
        _alloc = hooks.alloc;
        _free = hooks.free;
        _realloc = hooks.realloc;
        // Functions wrapper to remove lambda capture and convert to c interface (unsigned
        // int).
        auto alloc_wrapper = [](unsigned int bytes) -> void * {
            return _alloc(static_cast<size_t>(bytes));
        };

        auto free_wrapper = [](void *p) -> void { _free(p); };
        auto realloc_wrapper = [](void *p, unsigned int bytes) -> void * {
            return _realloc(p, static_cast<size_t>(bytes));
        };

        i3d_ping_allocator_set_alloc(alloc_wrapper);
        i3d_ping_allocator_set_free(free_wrapper);
        i3d_ping_allocator_set_realloc(realloc_wrapper);
    }

    //-----------------------
    // Create the i3D Ping Client.

    I3dPingError err = i3d_ping_pingers_create(&_pingers);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    // Set custom logger - optional.
    err = i3d_ping_pingers_set_logger(_pingers, log,
                                      nullptr);  // null userdata as global log is used.
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return false;
    }

    //---------------
    // Set callbacks.

    L_INFO("I3dPingersWrapper init complete");
    return true;
}

void I3dPingersWrapper::shutdown() {
    const std::lock_guard<std::mutex> lock(_wrapper);

    if (_pingers == nullptr) {
        return;
    }

    // Free all objects created via the ONE API. This also shuts down the server
    // first, ending any active connection to it.
    i3d_ping_pingers_destroy(_pingers);
    _pingers = nullptr;
}

void I3dPingersWrapper::update(bool quiet) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    assert(_pingers != nullptr);

    // Updates the server, which handles client connections, and services
    // outgoing and incoming messages.
    // Any registered callbacks will called during update, if the corresponding
    // messages are received.
    I3dPingError err = i3d_ping_pingers_update(_pingers);
    if (i3d_ping_is_error(err)) {
        if (!quiet) L_ERROR(i3d_ping_error_text(err));
        return;
    }
}

std::string I3dPingersWrapper::status_to_string(Status status) {
    switch (status) {
        case Status::uninitialized:
            return "uninitialized";
        case Status::initialized:
            return "initialized";
        default:
            return "unknown";
    }
}

I3dPingersWrapper::Status I3dPingersWrapper::status() const {
    const std::lock_guard<std::mutex> lock(_wrapper);

    I3dPingersStatus status;
    I3dPingError err = i3d_ping_pingers_status(_pingers, &status);
    if (i3d_ping_is_error(err)) {
        L_ERROR(i3d_ping_error_text(err));
        return Status::unknown;
    }
    switch (status) {
        case I3D_PINGERS_STATUS_UNINITIALIZED:
            return Status::uninitialized;
        case I3D_PINGERS_STATUS_INITIALIZED:
            return Status::initialized;
        default:
            return Status::unknown;
    }
}

}  // namespace i3d_ping_integration
