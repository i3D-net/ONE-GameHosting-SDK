#include <one/game/one_server_wrapper.h>

#include <one/arcus/c_api.h>
#include <one/arcus/c_error.h>

#include <one/game/log.h>

#include <assert.h>
#include <cstring>

namespace game {

OneServerWrapper::OneServerWrapper(unsigned int port)
    : _server(nullptr)
    , _live_state(nullptr)
    , _player_joined(nullptr)
    , _player_left(nullptr)
    , _host_information(nullptr)
    , _application_instance_information(nullptr)
    , _application_instance_get_status(nullptr)
    , _application_instance_set_status(nullptr)
    , _port(port)
    , host_information_request_sent(false)
    , application_instance_information_request_sent(false)
    , _game_state()
    , _last_update_game_state()
    , _soft_stop_callback(nullptr)
    , _soft_stop_userdata(nullptr)
    , _allocated_callback(nullptr)
    , _allocated_userdata(nullptr)
    , _meta_data_callback(nullptr)
    , _meta_data_userdata(nullptr)
    , _host_information_callback(nullptr)
    , _application_instance_information_callback(nullptr) {}

OneServerWrapper::~OneServerWrapper() {
    shutdown();
}

bool OneServerWrapper::init() {
    if (_server != nullptr || _live_state != nullptr || _player_joined != nullptr ||
        _player_left != nullptr || _host_information != nullptr ||
        _application_instance_information != nullptr ||
        _application_instance_get_status != nullptr ||
        _application_instance_set_status != nullptr) {
        L_ERROR("already init");
        return false;
    }

    // Create the one server. This example has a game server with a
    // corresponding arcus server. Multiple arcus servers may be created if
    // the process is hosting multiple game servers. Each game server must have
    // one corresponding arcus server.

    OneError err = one_server_create(&_server);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    // Create cached messages that can be sent as responses.

    err = one_message_create(&_live_state);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_message_create(&_player_joined);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_message_create(&_player_left);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_message_create(&_host_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_message_create(&_application_instance_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_message_create(&_application_instance_get_status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_message_create(&_application_instance_set_status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    //---------------
    // Set callbacks.

    // The default behavior of the platform will always hard stop your application, Its
    // required to configure a soft stop method for your application before this packet
    // will be send to your application.
    err = one_server_set_soft_stop_callback(_server, soft_stop, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_set_allocated_callback(_server, allocated, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_set_meta_data_callback(_server, meta_data, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_set_live_state_request_callback(_server, live_state_request, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_set_live_state_request_callback(_server, live_state_request, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_set_host_information_response_callback(_server, host_information,
                                                            this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_set_application_instance_information_response_callback(
        _server, application_instance_information, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_set_application_instance_get_status_response_callback(
        _server, application_instance_get_status, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_set_application_instance_set_status_response_callback(
        _server, application_instance_set_status, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    //------------------
    // Start the server.

    // Todo - retry listen loop in update if listen fails...

    err = one_server_listen(_server, _port);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    L_INFO("init done");
    return true;
}

void OneServerWrapper::shutdown() {
    if (_server == nullptr && _player_joined == nullptr && _player_left == nullptr &&
        _live_state == nullptr && _host_information == nullptr &&
        _application_instance_information == nullptr) {
        return;
    }

    one_server_destroy(&_server);
    one_message_destroy(&_player_joined);
    one_message_destroy(&_player_left);
    one_message_destroy(&_live_state);
    one_message_destroy(&_host_information);
    one_message_destroy(&_application_instance_information);
    one_message_destroy(&_application_instance_get_status);
    one_message_destroy(&_application_instance_set_status);
}

void OneServerWrapper::set_game_state(const GameState &state) {
    _game_state = state;
}

void OneServerWrapper::update() {
    assert(_server != nullptr);

    if (!host_information_request_sent) {
        if (send_host_information_request()) {
            host_information_request_sent = true;
        }
    }

    if (!application_instance_information_request_sent) {
        if (send_application_instance_information_request()) {
            application_instance_information_request_sent = true;
        }
    }

    if (_last_update_game_state.players != _game_state.players) {
        if (_last_update_game_state.players < _game_state.players) {
            if (!send_player_joined_event(_game_state.players)) {
                L_ERROR("failed to send player joined event response");
            }
        } else {
            if (!send_player_left(_game_state.players)) {
                L_ERROR("failed to send player left response");
            }
        }
    }

    OneError err = one_server_update(_server);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    _last_update_game_state = _game_state;
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
    }
    return "unknown";
}

OneServerWrapper::Status OneServerWrapper::status() const {
    int status = 0;
    OneError err = one_server_status(_server, &status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
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

void OneServerWrapper::set_soft_stop_callback(std::function<void(int, void *)> callback,
                                              void *userdata) {
    _soft_stop_callback = callback;
    _soft_stop_userdata = userdata;
}

void OneServerWrapper::set_allocated_callback(
    std::function<void(const AllocatedData &, void *)> callback, void *userdata) {
    _allocated_callback = callback;
    _allocated_userdata = userdata;
}

void OneServerWrapper::set_meta_data_callback(
    std::function<void(const MetaDataData &, void *)> callback, void *userdata) {
    _meta_data_callback = callback;
    _meta_data_userdata = userdata;
}

void OneServerWrapper::set_host_information_callback(
    std::function<void(HostInformationData)> callback) {
    _host_information_callback = callback;
}

void OneServerWrapper::set_application_instance_information_callback(
    std::function<void(ApplicationInstanceInformationData)> callback) {
    _application_instance_information_callback = callback;
}

void OneServerWrapper::set_application_instance_get_status_callback(
    std::function<void(ApplicationInstanceGetStatusData)> callback) {
    _application_instance_get_status_callback = callback;
}

void OneServerWrapper::set_application_instance_set_status_callback(
    std::function<void(ApplicationInstanceSetStatusData)> callback) {
    _application_instance_set_status_callback = callback;
}

bool OneServerWrapper::send_application_instance_get_status() {
    assert(_server != nullptr && _application_instance_get_status != nullptr);

    OneError err = one_message_prepare_application_instance_get_status_request(
        _application_instance_get_status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_send_application_instance_get_status_request(
        _server, _application_instance_get_status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    return true;
}

bool OneServerWrapper::send_application_instance_set_status(int status) {
    assert(_server != nullptr && _application_instance_set_status != nullptr);

    OneError err = one_message_prepare_application_instance_set_status_request(
        status, _application_instance_set_status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_send_application_instance_set_status_request(
        _server, _application_instance_set_status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    return true;
}

bool OneServerWrapper::send_player_joined_event(int num_players) {
    assert(_server != nullptr && _player_joined != nullptr);

    OneError err =
        one_message_prepare_player_joined_event_response(num_players, _player_joined);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_send_player_joined_event_response(_server, _player_joined);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    return true;
}

bool OneServerWrapper::send_player_left(int num_players) {
    assert(_server != nullptr && _player_left != nullptr);

    OneError err = one_message_prepare_player_left_response(num_players, _player_left);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_send_player_left_response(_server, _player_left);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    return true;
}

bool OneServerWrapper::send_host_information_request() {
    assert(_server != nullptr && _host_information != nullptr);

    OneError err = one_message_prepare_host_information_request(_host_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_send_host_information_request(_server, _host_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    return true;
}

bool OneServerWrapper::send_application_instance_information_request() {
    assert(_server != nullptr && _application_instance_information != nullptr);

    OneError err = one_message_prepare_application_instance_information_request(
        _application_instance_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_send_application_instance_information_request(
        _server, _application_instance_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    return true;
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

    L_INFO("invoking soft stop callback");
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

    L_INFO("invoking allocated callback");
    wrapper->_allocated_callback(allocated_payload, wrapper->_allocated_userdata);
}

void OneServerWrapper::meta_data(void *userdata, void *meta_data) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    if (meta_data == nullptr) {
        L_ERROR("meta_data is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr);

    if (wrapper->_meta_data_callback == nullptr) {
        L_INFO("meta data callback is nullptr");
        return;
    }

    auto array = reinterpret_cast<OneArrayPtr>(meta_data);
    MetaDataData meta_data_payload;
    if (!extract_meta_data_payload(array, meta_data_payload)) {
        L_ERROR("failed to extract meta data payload");
        return;
    }

    L_INFO("invoking meta data callback");
    wrapper->_meta_data_callback(meta_data_payload, wrapper->_meta_data_userdata);
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

    auto object = reinterpret_cast<OneObjectPtr>(meta_data);
    HostInformationData information_payload;
    if (!extract_host_information_payload(object, information_payload)) {
        L_ERROR("failed to extract host information payload");
        return;
    }

    L_INFO("invoking host information callback");
    wrapper->_host_information_callback(information_payload);
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
        L_INFO("host information callback is nullptr");
        return;
    }

    auto object = reinterpret_cast<OneObjectPtr>(meta_data);
    ApplicationInstanceInformationData information_payload;
    if (!extract_application_instance_information_payload(object, information_payload)) {
        L_ERROR("failed to extract host information payload");
        return;
    }

    L_INFO("invoking host information callback");
    wrapper->_application_instance_information_callback(information_payload);
}

void OneServerWrapper::application_instance_get_status(void *userdata, int status) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr);

    if (wrapper->_application_instance_information_callback == nullptr) {
        L_INFO("host information callback is nullptr");
        return;
    }

    L_INFO("invoking host information callback");
    ApplicationInstanceGetStatusData data;
    data.status = status;
    wrapper->_application_instance_get_status_callback(data);
}

void OneServerWrapper::application_instance_set_status(void *userdata, int code) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr);

    if (wrapper->_application_instance_information_callback == nullptr) {
        L_INFO("host information callback is nullptr");
        return;
    }

    L_INFO("invoking host information callback");
    ApplicationInstanceSetStatusData data;
    data.code = code;
    wrapper->_application_instance_set_status_callback(data);
}

void OneServerWrapper::live_state_request(void *userdata) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr && wrapper->_live_state != nullptr);

    L_INFO("invoking live state request callback");
    const auto &state = wrapper->_game_state;
    OneError err = one_message_prepare_live_state_response(
        state.players, state.max_players, state.name.c_str(), state.map.c_str(),
        state.mode.c_str(), state.version.c_str(), wrapper->_live_state);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_server_send_live_state_response(wrapper->_server, wrapper->_live_state);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }
}

bool OneServerWrapper::extract_allocated_payload(OneArrayPtr array,
                                                 AllocatedData &allocated_data) {
    if (array == nullptr) {
        L_ERROR("array is nullptr");
        return false;
    }

    bool is_map_set = false;
    bool is_max_player_set = false;

    size_t size = 0;
    auto err = one_array_size(array, &size);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    OneObjectPtr pair = nullptr;
    err = one_object_create(&pair);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    for (unsigned int pos = 0; pos < size; ++pos) {
        err = one_array_val_object(array, pos, pair);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            return false;
        }

        size_t key_size = 0;
        err = one_object_val_string_size(pair, "key", &key_size);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            return false;
        }

        char *key = new char[key_size];
        if (key == nullptr) {
            L_ERROR("key is nullptr");
            one_object_destroy(&pair);
            return false;
        }

        err = one_object_val_string(pair, "key", key, key_size);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            delete[] key;
            return false;
        }

        size_t value_size = 0;
        err = one_object_val_string_size(pair, "value", &value_size);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            delete[] key;
            return false;
        }

        char *value = new char[value_size];
        if (value == nullptr) {
            L_ERROR("value is nullptr");
            one_object_destroy(&pair);
            delete[] key;
            return false;
        }

        err = one_object_val_string(pair, "value", value, value_size);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            delete[] key;
            delete[] value;
            return false;
        }

        if (std::strncmp(key, "map", key_size) == 0) {
            allocated_data.map = std::string(value);
            is_map_set = true;
        } else if (std::strncmp(key, "maxPlayer", key_size) == 0) {
            allocated_data.max_players = std::string(value);
            is_max_player_set = true;
        }

        delete[] key;
        delete[] value;
    }

    one_object_destroy(&pair);

    if (!is_map_set) {
        L_ERROR("map is missing from payload");
        return false;
    }

    if (!is_max_player_set) {
        L_ERROR("max_player is missing from payload");
        return false;
    }

    return true;
}

bool OneServerWrapper::extract_meta_data_payload(OneArrayPtr array,
                                                 MetaDataData &meta_data) {
    if (array == nullptr) {
        L_ERROR("array is nullptr");
        return false;
    }

    bool is_map_set = false;
    bool is_mode_set = false;
    bool is_type_set = false;

    size_t size = 0;
    auto err = one_array_size(array, &size);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    OneObjectPtr pair = nullptr;
    err = one_object_create(&pair);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    for (unsigned int pos = 0; pos < size; ++pos) {
        err = one_array_val_object(array, pos, pair);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            return false;
        }

        size_t key_size = 0;
        err = one_object_val_string_size(pair, "key", &key_size);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            return false;
        }

        char *key = new char[key_size];
        if (key == nullptr) {
            L_ERROR("key is nullptr");
            one_object_destroy(&pair);
            return false;
        }

        err = one_object_val_string(pair, "key", key, key_size);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            delete[] key;
            return false;
        }

        size_t value_size = 0;
        err = one_object_val_string_size(pair, "value", &value_size);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            delete[] key;
            return false;
        }

        char *value = new char[value_size];
        if (value == nullptr) {
            L_ERROR("value is nullptr");
            one_object_destroy(&pair);
            delete[] key;
            return false;
        }

        err = one_object_val_string(pair, "value", value, value_size);
        if (is_error(err)) {
            L_ERROR(error_text(err));
            one_object_destroy(&pair);
            delete[] key;
            delete[] value;
            return false;
        }

        if (std::strncmp(key, "map", key_size) == 0) {
            meta_data.map = std::string(value);
            is_map_set = true;
        } else if (std::strncmp(key, "mode", key_size) == 0) {
            meta_data.mode = std::string(value);
            is_mode_set = true;
        } else if (std::strncmp(key, "type", key_size) == 0) {
            meta_data.type = std::string(value);
            is_type_set = true;
        }

        delete[] key;
        delete[] value;
    }

    one_object_destroy(&pair);

    if (!is_map_set) {
        L_ERROR("map is missing from payload");
        return false;
    }

    if (!is_mode_set) {
        L_ERROR("mode is missing from payload");
        return false;
    }

    if (!is_type_set) {
        L_ERROR("type is missing from payload");
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

    auto err = one_object_val_int(object, "id", &information.id);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_object_val_int(object, "serverId", &information.server_id);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    size_t size = 0;
    err = one_object_val_string_size(object, "serverName", &size);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    char *temp = new char[size];
    if (temp == nullptr) {
        L_ERROR("temp is nullptr");
        return false;
    }

    err = one_object_val_string(object, "serverName", temp, size);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    information.server_name = temp;
    delete[] temp;

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

    size_t size = 0;
    auto err = one_object_val_string_size(object, "fleetId", &size);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    char *temp = new char[size];
    if (temp == nullptr) {
        L_ERROR("temp is nullptr");
        return false;
    }

    err = one_object_val_string(object, "fleetId", temp, size);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    information.fleet_id = temp;
    delete[] temp;

    err = one_object_val_int(object, "hostId", &information.host_id);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_object_val_int(object, "isVirtual", &information.is_virtual);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    // ... add more field parsing as needed.

    return true;
}

bool TestOneServerWrapper::extract_allocated_payload(
    OneArrayPtr array, OneServerWrapper::AllocatedData &allocated_data) {
    return OneServerWrapper::extract_allocated_payload(array, allocated_data);
}

bool TestOneServerWrapper::extract_meta_data_payload(
    OneArrayPtr array, OneServerWrapper::MetaDataData &meta_data) {
    return OneServerWrapper::extract_meta_data_payload(array, meta_data);
}

}  // namespace game
