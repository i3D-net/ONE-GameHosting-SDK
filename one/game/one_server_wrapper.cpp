#include <one/game/one_server_wrapper.h>

#include <one/arcus/c_api.h>
#include <one/arcus/c_error.h>
#include <one/game/log.h>
#include <one/game/parsing.h>

#include <assert.h>
#include <cstring>

namespace game {

OneServerWrapper::OneServerWrapper(unsigned int port)
    : _server(nullptr)
    , _port(port)
    , _live_state(nullptr)
    , _player_joined(nullptr)
    , _player_left(nullptr)
    , _host_information(nullptr)
    , _application_instance_information(nullptr)
    , _application_instance_get_status(nullptr)
    , _application_instance_set_status(nullptr)
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
    , _host_information_userdata(nullptr)
    , _application_instance_information_callback(nullptr)
    , _application_instance_information_userdata(nullptr)
    , _application_instance_get_status_callback(nullptr)
    , _application_instance_get_status_userdata(nullptr)
    , _application_instance_set_status_callback(nullptr)
    , _application_instance_set_status_userdata(nullptr) {}

OneServerWrapper::~OneServerWrapper() {
    shutdown();
}

bool OneServerWrapper::init() {
    const std::lock_guard<std::mutex> lock(_wrapper);

    if (_server != nullptr || _live_state != nullptr || _player_joined != nullptr ||
        _player_left != nullptr || _host_information != nullptr ||
        _application_instance_information != nullptr ||
        _application_instance_get_status != nullptr ||
        _application_instance_set_status != nullptr) {
        L_ERROR("already init");
        return false;
    }

    //-----------------------
    // Create the one server.

    // Each game server must have one corresponding arcus server.
    OneError err = one_server_create(&_server);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    //------------------------
    // Create cached messages.

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

    err = one_server_listen(_server, _port);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    L_INFO("OneServerWrapper init complete");
    return true;
}

void OneServerWrapper::shutdown() {
    const std::lock_guard<std::mutex> lock(_wrapper);

    if (_server == nullptr && _player_joined == nullptr && _player_left == nullptr &&
        _live_state == nullptr && _host_information == nullptr &&
        _application_instance_information == nullptr) {
        return;
    }

    // Free all objects created via the One API.
    one_server_destroy(_server);
    _server = nullptr;
    one_message_destroy(_player_joined);
    _player_joined = nullptr;
    one_message_destroy(_player_left);
    _player_left = nullptr;
    one_message_destroy(_live_state);
    _live_state = nullptr;
    one_message_destroy(_host_information);
    _host_information = nullptr;
    one_message_destroy(_application_instance_information);
    _application_instance_information = nullptr;
    one_message_destroy(_application_instance_get_status);
    _application_instance_get_status = nullptr;
    one_message_destroy(_application_instance_set_status);
    _application_instance_set_status = nullptr;
}

void OneServerWrapper::set_game_state(const GameState &state) {
    const std::lock_guard<std::mutex> lock(_wrapper);

    _game_state = state;
}

void OneServerWrapper::update() {
    const std::lock_guard<std::mutex> lock(_wrapper);

    assert(_server != nullptr);

    // Requesting host information request only once at startup.
    // The agent will reply with the host information message providing
    // the host information details.
    // Retry sending the message until it is succesfully put in the outgoing message
    // queue.
    if (!host_information_request_sent) {
        if (send_host_information_request()) {
            host_information_request_sent = true;
        }
    }

    // Requesting application instance information request only once at startup.
    // The agent will reply with the host information message providing
    // the host information details.
    // Retry sending the message until it is succesfully put in the outgoing message
    // queue.
    if (!application_instance_information_request_sent) {
        if (send_application_instance_information_request()) {
            application_instance_information_request_sent = true;
        }
    }

    // Keeping track of player count changes & send out the appropriate
    // player joined or player left message.
    if (_last_update_game_state.players != _game_state.players) {
        if (_last_update_game_state.players < _game_state.players) {
            L_INFO("player joined");
            if (!send_player_joined_event(_game_state.players)) {
                L_ERROR("failed to send player joined event response");
            }
        } else {
            L_INFO("player left");
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

    // Updating the last sent game state to keep track of changes occuring inbetween
    // update calls.
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
        default:
            return "unknown";
    }
}

OneServerWrapper::Status OneServerWrapper::status() const {
    OneServerStatus status;
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

void OneServerWrapper::set_soft_stop_callback(
    std::function<void(int timeout, void *userdata)> callback, void *userdata) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    _soft_stop_callback = callback;
    _soft_stop_userdata = userdata;
}

void OneServerWrapper::set_allocated_callback(
    std::function<void(const AllocatedData &data, void *userdata)> callback,
    void *userdata) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    _allocated_callback = callback;
    _allocated_userdata = userdata;
}

void OneServerWrapper::set_meta_data_callback(
    std::function<void(const MetaDataData &data, void *userdata)> callback,
    void *userdata) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    _meta_data_callback = callback;
    _meta_data_userdata = userdata;
}

void OneServerWrapper::set_host_information_callback(
    std::function<void(const HostInformationData &data, void *userdata)> callback,
    void *userdata) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    _host_information_callback = callback;
    _host_information_userdata = userdata;
}

void OneServerWrapper::set_application_instance_information_callback(
    std::function<void(const ApplicationInstanceInformationData &data, void *userdata)>
        callback,
    void *userdata) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    _application_instance_information_callback = callback;
    _application_instance_information_userdata = userdata;
}

void OneServerWrapper::set_application_instance_get_status_callback(
    std::function<void(const ApplicationInstanceGetStatusData &data, void *userdata)>
        callback,
    void *userdata) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    _application_instance_get_status_callback = callback;
    _application_instance_get_status_userdata = userdata;
}

void OneServerWrapper::set_application_instance_set_status_callback(
    std::function<void(const ApplicationInstanceSetStatusData &data, void *userdata)>
        callback,
    void *userdata) {
    const std::lock_guard<std::mutex> lock(_wrapper);
    _application_instance_set_status_callback = callback;
    _application_instance_set_status_userdata = userdata;
}

bool OneServerWrapper::send_application_instance_get_status() {
    assert(_server != nullptr && _application_instance_get_status != nullptr);
    const std::lock_guard<std::mutex> lock(_wrapper);

    OneError err = one_message_prepare_application_instance_get_status_request(
        _application_instance_get_status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_send_application_instance_get_status_request(
        _server, _application_instance_get_status);
    if (is_error(err)) {

        return false;
    }

    return true;
}

bool OneServerWrapper::send_application_instance_set_status(StatusCode status) {
    assert(_server != nullptr && _application_instance_set_status != nullptr);
    const std::lock_guard<std::mutex> lock(_wrapper);

    OneError err = one_message_prepare_application_instance_set_status_request(
        static_cast<int>(status), _application_instance_set_status);
    if (is_error(err)) {
        L_ERROR(error_text(err));
        return false;
    }

    err = one_server_send_application_instance_set_status_request(
        _server, _application_instance_set_status);
    if (is_error(err)) {
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

    auto object = reinterpret_cast<OneObjectPtr>(information);
    HostInformationData information_payload;
    if (!extract_host_information_payload(object, information_payload)) {
        L_ERROR("failed to extract host information payload");
        return;
    }

    L_INFO("invoking host information callback");
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
        L_ERROR("failed to extract host information payload");
        return;
    }

    L_INFO("invoking application instance information callback");
    wrapper->_application_instance_information_callback(
        information_payload, wrapper->_application_instance_information_userdata);
}

void OneServerWrapper::application_instance_get_status(void *userdata, int status) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr);

    if (wrapper->_application_instance_get_status_callback == nullptr) {
        L_INFO("application instance get status callback is nullptr");
        return;
    }

    L_INFO("invoking application instance get status callback");
    ApplicationInstanceGetStatusData data;
    data.status = status;
    wrapper->_application_instance_get_status_callback(
        data, wrapper->_application_instance_get_status_userdata);
}

void OneServerWrapper::application_instance_set_status(void *userdata, int code) {
    if (userdata == nullptr) {
        L_ERROR("userdata is nullptr");
        return;
    }

    auto wrapper = reinterpret_cast<OneServerWrapper *>(userdata);
    assert(wrapper->_server != nullptr);

    if (wrapper->_application_instance_set_status_callback == nullptr) {
        L_INFO("application instance information set status is nullptr");
        return;
    }

    L_INFO("invoking application instance set status callback");
    ApplicationInstanceSetStatusData data;
    data.code = static_cast<OneServerWrapper::StatusSetCodeResult>(code);
    wrapper->_application_instance_set_status_callback(
        data, wrapper->_application_instance_set_status_userdata);
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

    auto callback = [&](const size_t total_number_of_keys, const std::string &key,
                        const std::string &value) {
        if (total_number_of_keys != 2) {
            L_ERROR("got total number of keys(" + std::to_string(total_number_of_keys) +
                    ") expected 2 instead");
            return false;
        }

        if (key == "map") {
            allocated_data.map = value;
            return true;
        }

        if (key == "maxPlayers") {
            allocated_data.max_players = value;
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

// Todo: can these extract_xxx functions be done within the API itself?
bool OneServerWrapper::extract_meta_data_payload(OneArrayPtr array,
                                                 MetaDataData &meta_data) {
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
            meta_data.map = value;
            return true;
        }

        if (key == "mode") {
            meta_data.mode = value;
            return true;
        }

        if (key == "type") {
            meta_data.type = value;
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
        L_ERROR("failed to extract fleetId key");
        return false;
    }

    auto err = one_object_val_int(object, "hostId", &information.host_id);
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

}  // namespace game
