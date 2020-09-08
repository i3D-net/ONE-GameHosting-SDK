#include <one/game/one_server_wrapper.h>

#include <one/arcus/c_api.h>
#include <one/arcus/c_error.h>

#include <one/game/log.h>

#include <assert.h>
#include <cstring>

namespace game {

OneServerWrapper::OneServerWrapper(unsigned int port)
    : _server(nullptr)
    , _error(nullptr)
    , _live_state(nullptr)
    , _host_information(nullptr)
    , _allocated(nullptr)
    , _meta_data(nullptr)
    , _port(port)
    , host_information_request_sent(false)
    , _game_state()
    , _soft_stop_callback(nullptr)
    , _allocated_callback(nullptr)
    , _meta_data_callback(nullptr) {}

OneServerWrapper::~OneServerWrapper() {
    shutdown();
}

void OneServerWrapper::init() {
    if (_server != nullptr || _error != nullptr || _live_state != nullptr ||
        _host_information != nullptr) {
        L_ERROR("already init");
        return;
    }

    // Create the one server. This example has a game server with a
    // corresponding arcus server. Multiple arcus servers may be created if
    // the process is hosting multiple game servers. Each game server must have
    // one corresponding arcus server.

    OneError err = one_server_create(&_server);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    // Create cached messages that can be sent as responses.

    err = one_message_create(&_error);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_message_create(&_live_state);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_message_create(&_host_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_array_create(&_allocated);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_array_create(&_meta_data);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    //---------------
    // Set callbacks.

    // The default behavior of the platform will always hard stop your application, Its
    // required to configure a soft stop method for your application before this packet
    // will be send to your application.
    err = one_server_set_soft_stop_callback(_server, soft_stop, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_server_set_allocated_callback(_server, allocated, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_server_set_meta_data_callback(_server, meta_data, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_server_set_live_state_request_callback(_server, live_state_request, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_server_set_live_state_request_callback(_server, live_state_request, this);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    //------------------
    // Start the server.

    // Todo - retry listen loop in update if listen fails...

    err = one_server_listen(_server, _port);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    L_INFO("init done");
}

void OneServerWrapper::shutdown() {
    if (_server == nullptr && _error == nullptr && _live_state == nullptr &&
        _host_information == nullptr) {
        return;
    }

    one_server_destroy(&_server);
    one_message_destroy(&_error);
    one_message_destroy(&_live_state);
    one_message_destroy(&_host_information);
    one_array_destroy(&_allocated);
    one_array_destroy(&_meta_data);
}

void OneServerWrapper::set_game_state(const GameState &state) {
    _game_state = state;
}

void OneServerWrapper::update() {
    assert(_server != nullptr);

    // Add a host_information_request in the message outbound queue.
    if (!host_information_request_sent) {
        if (send_host_information_request()) {
            host_information_request_sent = true;
        }
    }

    OneError err = one_server_update(_server);
    if (is_error(err)) {
        L_ERROR(error_text(err));
    }
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

void OneServerWrapper::set_soft_stop_callback(std::function<void(int)> callback) {
    _soft_stop_callback = callback;
}

void OneServerWrapper::set_allocated_callback(
    std::function<void(AllocatedData)> callback) {
    _allocated_callback = callback;
}

void OneServerWrapper::set_meta_data_callback(
    std::function<void(MetaDataData)> callback) {
    _meta_data_callback = callback;
}

void OneServerWrapper::send_error_response() {
    assert(_server != nullptr && _error != nullptr);

    OneError err = one_message_prepare_error_response(_host_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }

    err = one_server_send_error_response(_server, _host_information);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return;
    }
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

    wrapper->_soft_stop_callback(timeout_seconds);
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
    assert(wrapper->_server != nullptr && wrapper->_allocated != nullptr);

    if (wrapper->_allocated_callback == nullptr) {
        L_INFO("allocated callback is nullptr");
        return;
    }

    auto array = reinterpret_cast<OneArrayPtr>(allocated);

    AllocatedData allocated_payload = {0};
    if (!extract_allocated_payload(array, allocated_payload)) {
        L_ERROR("failed to extract allocated payload");
        return;
    }

    wrapper->_allocated_callback(allocated_payload);
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
    assert(wrapper->_server != nullptr && wrapper->_meta_data != nullptr);

    if (wrapper->_meta_data_callback == nullptr) {
        L_INFO("meta data callback is nullptr");
        return;
    }

    auto array = reinterpret_cast<OneArrayPtr>(meta_data);
    MetaDataData meta_data_payload = {0};
    if (!extract_meta_data_payload(array, meta_data_payload)) {
        L_ERROR("failed to extract meta data payload");
        return;
    }

    wrapper->_meta_data_callback(meta_data_payload);
}

void OneServerWrapper::live_state_request(void *userdata) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr && wrapper->_live_state != nullptr);

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

}  // namespace game
