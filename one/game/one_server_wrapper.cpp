#include <one/game/one_server_wrapper.h>

#include <assert.h>
#include <string>
#include <cstring>

#include <one/arcus/c_api.h>
#include <one/arcus/c_error.h>
#include <one/game/log.h>
#include <one/game/parsing.h>

namespace one_integration {
namespace {

// Local cached memory function overrides, to assist in override pattern.
std::function<void *(size_t)> _alloc = nullptr;
std::function<void(void *)> _free = nullptr;
std::function<void *(void *, size_t)> _realloc = nullptr;

// Logger to pass into One.
void log(OneLogLevel level, const char *message) {
    switch (level) {
        case ONE_LOG_LEVEL_INFO: {
            L_INFO(std::string("ONELOG: ") + message);
            break;
        }
        case ONE_LOG_LEVEL_ERROR: {
            L_ERROR(std::string("ONELOG: ") + message);
            break;
        }
        default: {
            L_ERROR(std::string("ONELOG: unknown log level: ") + message);
        }
    }
}

}  // namespace

OneServerWrapper::OneServerWrapper()
    : _server(nullptr)
    , _soft_stop_callback(nullptr)
    , _soft_stop_userdata(nullptr)
    , _allocated_callback(nullptr)
    , _allocated_userdata(nullptr)
    , _metadata_callback(nullptr)
    , _metadata_userdata(nullptr)
    , _host_information_callback(nullptr)
    , _host_information_userdata(nullptr)
    , _application_instance_information_callback(nullptr)
    , _application_instance_information_userdata(nullptr) {}

OneServerWrapper::~OneServerWrapper() {
    shutdown();
}

bool OneServerWrapper::init(std::function<void *(size_t)> alloc,
                            std::function<void(void *)> free,
                            std::function<void *(void *, size_t)> realloc) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    if (_server != nullptr) {
        L_ERROR("already initialized");
        return false;
    }

    //----------------------
    // Set custom allocator.

    if (alloc && free && realloc) {
        // Cache off the overrides so that they can be called within the lambdas
        // because lambdas with captures may not be passed to the C API.
        _alloc = alloc;
        _free = free;
        _realloc = realloc;
        // Functions wrapper to remove lambda capture and convert to c interface (unsigned
        // int).
        auto alloc_wrapper = [](unsigned int bytes) -> void * {
            return _alloc(static_cast<size_t>(bytes));
        };
        auto free_wrapper = [](void *p) -> void { _free(p); };
        auto realloc_wrapper = [](void *p, unsigned int bytes) -> void * {
            return _realloc(p, static_cast<size_t>(bytes));
        };

        one_allocator_set_alloc(alloc_wrapper);
        one_allocator_set_free(free_wrapper);
        one_allocator_set_realloc(realloc_wrapper);
    }

    //-----------------------
    // Create the one server.

    // Each game server must have one corresponding arcus server.
    OneError err = one_server_create(log, &_server);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return false;
    }

    //---------------
    // Set callbacks.

    // The default behavior of the platform will always hard stop your application, Its
    // required to configure a soft stop method for your application before this packet
    // will be send to your application.
    err = one_server_set_soft_stop_callback(_server, soft_stop, this);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return false;
    }

    err = one_server_set_allocated_callback(_server, allocated, this);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return false;
    }

    err = one_server_set_metadata_callback(_server, metadata, this);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return false;
    }

    err = one_server_set_host_information_callback(_server, host_information, this);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return false;
    }

    err = one_server_set_application_instance_information_callback(
        _server, application_instance_information, this);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return false;
    }

    L_INFO("OneServerWrapper init complete");
    return true;
}

bool OneServerWrapper::listen(unsigned int port) {
    OneError err = one_server_listen(_server, port);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return false;
    }
    return true;
}

void OneServerWrapper::shutdown() {
    const std::lock_guard<std::mutex> lock(_wrapper);

    if (_server == nullptr) {
        return;
    }

    // Free all objects created via the ONE API. This also shuts down the server
    // first, ending any active connection to it.
    one_server_destroy(_server);
    _server = nullptr;
}

void OneServerWrapper::update(bool quiet) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    assert(_server != nullptr);

    // Updates the server, which handles client connections, and services
    // outgoing and incoming messages.
    // Any registered callbacks will called during update, if the corresponding
    // messages are received.
    OneError err = one_server_update(_server);
    if (one_is_error(err)) {
        if (!quiet) L_ERROR(one_error_text(err));
        return;
    }
}

std::string OneServerWrapper::status_to_string(Status status) {
    switch (status) {
        case Status::uninitialized:
            return "uninitialized";
        case Status::initialized:
            return "initialized";
        case Status::waiting_for_client:
            return "waiting_for_client";
        case Status::handshake:
            return "handshake";
        case Status::ready:
            return "ready";
        case Status::error:
            return "error";
        default:
            return "unknown";
    }
}

OneServerWrapper::Status OneServerWrapper::status() const {
    const std::lock_guard<std::mutex> lock(_wrapper);

    OneServerStatus status;
    OneError err = one_server_status(_server, &status);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return Status::unknown;
    }
    switch (status) {
        case ONE_SERVER_STATUS_UNINITIALIZED:
            return Status::uninitialized;
        case ONE_SERVER_STATUS_INITIALIZED:
            return Status::initialized;
        case ONE_SERVER_STATUS_WAITING_FOR_CLIENT:
            return Status::waiting_for_client;
        case ONE_SERVER_STATUS_HANDSHAKE:
            return Status::handshake;
        case ONE_SERVER_STATUS_READY:
            return Status::ready;
        case ONE_SERVER_STATUS_ERROR:
            return Status::error;
        default:
            return Status::unknown;
    }
}

void OneServerWrapper::set_game_state(const GameState &state) {
    // If the game wishes to send and coordinate the processing of additional
    // game state to the ONE Platform, it can add that data here as an object
    // with additional keys. Note that these key names must not conflict with
    // the base key names of the Live State message.
    // OneObjectPtr object;
    // OneError err = one_object_create(&object);
    // if (one_is_error(err)) {
    //     L_ERROR(one_error_text(err));
    // }
    // one_object_set_val_int(object, "matchLength", 123);

    // The optional object may be passed here as the last variable.
    OneError err = one_server_set_live_state(
        _server, state.players, state.max_players, state.name.c_str(), state.map.c_str(),
        state.mode.c_str(), state.version.c_str(), nullptr);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
    }

    // If custom data was added, then make sure to destroy the object.
    // one_object_destroy(object);
}

void OneServerWrapper::set_application_instance_status(ApplicationInstanceStatus status) {
    OneError err = one_server_set_application_instance_status(
        _server, (OneApplicationInstanceStatus)status);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
    }
}

void OneServerWrapper::set_soft_stop_callback(
    std::function<void(int timeout, void *userdata)> callback, void *userdata) {

    _soft_stop_callback = callback;
    _soft_stop_userdata = userdata;
}

void OneServerWrapper::set_allocated_callback(
    std::function<void(const AllocatedData &data, void *userdata)> callback,
    void *userdata) {

    _allocated_callback = callback;
    _allocated_userdata = userdata;
}

void OneServerWrapper::set_metadata_callback(
    std::function<void(const MetaDataData &data, void *userdata)> callback,
    void *userdata) {
    _metadata_callback = callback;
    _metadata_userdata = userdata;
}

void OneServerWrapper::set_host_information_callback(
    std::function<void(const HostInformationData &data, void *userdata)> callback,
    void *userdata) {
    _host_information_callback = callback;
    _host_information_userdata = userdata;
}

void OneServerWrapper::set_application_instance_information_callback(
    std::function<void(const ApplicationInstanceInformationData &data, void *userdata)>
        callback,
    void *userdata) {
    _application_instance_information_callback = callback;
    _application_instance_information_userdata = userdata;
}

// Tell the server to shutdown at the next appropriate time for its users (e.g.,
// after a match end).
void OneServerWrapper::soft_stop(void *userdata, int timeout_seconds) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    if (wrapper->_soft_stop_callback == nullptr) {
        L_INFO("soft stop callback is nullptr");
        return;
    }

    // Leave userdata optional.
    wrapper->_soft_stop_callback(timeout_seconds, wrapper->_soft_stop_userdata);
}

void OneServerWrapper::allocated(void *userdata, void *allocated) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    if (allocated == nullptr) {
        L_ERROR("allocated is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr);

    if (wrapper->_allocated_callback == nullptr) {
        L_INFO("allocated callback is nullptr");
        return;
    }

    auto array = reinterpret_cast<OneArrayPtr>(allocated);

    AllocatedData allocated_payload;
    if (!extract_allocated_payload(array, allocated_payload)) {
        L_ERROR("failed to extract allocated payload");
        return;
    }

    wrapper->_allocated_callback(allocated_payload, wrapper->_allocated_userdata);
}

void OneServerWrapper::metadata(void *userdata, void *metadata) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    if (metadata == nullptr) {
        L_ERROR("metadata is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr);

    if (wrapper->_metadata_callback == nullptr) {
        L_INFO("meta data callback is nullptr");
        return;
    }

    auto array = reinterpret_cast<OneArrayPtr>(metadata);
    MetaDataData metadata_payload;
    if (!extract_metadata_payload(array, metadata_payload)) {
        L_ERROR("failed to extract meta data payload");
        return;
    }

    wrapper->_metadata_callback(metadata_payload, wrapper->_metadata_userdata);
}

void OneServerWrapper::host_information(void *userdata, void *information) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    if (information == nullptr) {
        L_ERROR("information is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr);

    if (wrapper->_host_information_callback == nullptr) {
        L_INFO("host information callback is nullptr");
        return;
    }

    auto object = reinterpret_cast<OneObjectPtr>(information);
    HostInformationData information_payload;
    if (!extract_host_information_payload(object, information_payload)) {
        L_ERROR("failed to extract host information payload");
        return;
    }

    wrapper->_host_information_callback(information_payload,
                                        wrapper->_host_information_userdata);
}

void OneServerWrapper::application_instance_information(void *userdata,
                                                        void *information) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    if (information == nullptr) {
        L_ERROR("information is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr);

    if (wrapper->_application_instance_information_callback == nullptr) {
        L_INFO("application instance information callback is nullptr");
        return;
    }

    auto object = reinterpret_cast<OneObjectPtr>(information);
    ApplicationInstanceInformationData information_payload;
    if (!extract_application_instance_information_payload(object, information_payload)) {
        L_ERROR("failed to extract host application instance information payload");
        return;
    }

    L_INFO("invoking application instance information callback");
    wrapper->_application_instance_information_callback(
        information_payload, wrapper->_application_instance_information_userdata);
}

bool OneServerWrapper::extract_allocated_payload(OneArrayPtr array,
                                                 AllocatedData &allocated_data) {
    if (array == nullptr) {
        L_ERROR("array is nullptr");
        return false;
    }

    // Optional - the game can require and read allocated keys to configure
    // the server. This is to mirror the documentation example here:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#allocated
    auto callback = [&](const size_t total_number_of_keys, const std::string &key,
                        const std::string &value) {
        if (total_number_of_keys != 2) {
            L_ERROR("got total number of keys(" + std::to_string(total_number_of_keys) +
                    ") expected 2 instead");
            return false;
        }

        if (key == "players") {
            allocated_data.players = std::stoi(value);
            return true;
        }

        if (key == "duration") {
            allocated_data.duration = std::stoi(value);
            return true;
        }

        L_ERROR("key(" + key + ") is not handled");
        return false;
    };

    if (!Parsing::extract_key_value_payload(array, callback)) {
        L_ERROR("failed to extract key/value payload");
        return false;
    }

    return true;
}

bool OneServerWrapper::extract_metadata_payload(OneArrayPtr array,
                                                MetaDataData &metadata) {
    if (array == nullptr) {
        L_ERROR("array is nullptr");
        return false;
    }

    auto callback = [&](const size_t total_number_of_keys, const std::string &key,
                        const std::string &value) {
        if (total_number_of_keys != 3) {
            L_ERROR("got total number of keys(" + std::to_string(total_number_of_keys) +
                    ") expected 3 instead");
            return false;
        }

        if (key == "map") {
            metadata.map = value;
            return true;
        }

        if (key == "mode") {
            metadata.mode = value;
            return true;
        }

        if (key == "type") {
            metadata.type = value;
            return true;
        }

        L_ERROR("key(" + key + ") is not handled");
        return false;
    };

    if (!Parsing::extract_key_value_payload(array, callback)) {
        L_ERROR("failed to extract key/value payload");
        return false;
    }

    return true;
}

bool OneServerWrapper::extract_host_information_payload(
    OneObjectPtr object, OneServerWrapper::HostInformationData &information) {
    if (object == nullptr) {
        L_ERROR("object is nullptr.");
        return false;
    }

    OneError err = one_object_val_int(object, "id", &information.id);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return false;
    }

    err = one_object_val_int(object, "serverId", &information.server_id);
    if (one_is_error(err)) {
        L_ERROR(one_error_text(err));
        return false;
    }

    if (!Parsing::extract_string(object, "serverName", [&](const std::string &value) {
            information.server_name = value;
            return true;
        })) {
        L_ERROR("failed to extract serverName key");
        return false;
    }

    // ... add more field parsing as needed.

    return true;
}

bool OneServerWrapper::extract_application_instance_information_payload(
    OneObjectPtr object,
    OneServerWrapper::ApplicationInstanceInformationData &information) {
    if (object == nullptr) {
        L_ERROR("object is nullptr.");
        return false;
    }

    if (!Parsing::extract_string(object, "fleetId", [&](const std::string &value) {
            information.fleet_id = value;
            return true;
        })) {
        L_ERROR("failed to extract fleetId");
        return false;
    }

    OneError err = one_object_val_int(object, "hostId", &information.host_id);
    if (one_is_error(err)) {
        L_ERROR("failed to extract hostId");
        L_ERROR(one_error_text(err));
        return false;
    }

    err = one_object_val_bool(object, "isVirtual", &information.is_virtual);
    if (one_is_error(err)) {
        L_ERROR("failed to extract isVirtual");
        L_ERROR(one_error_text(err));
        return false;
    }

    // ... add more field parsing as needed.

    return true;
}

}  // namespace one_integration
