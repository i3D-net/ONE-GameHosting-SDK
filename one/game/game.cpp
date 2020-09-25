#include <one/game/game.h>

#include <assert.h>
#include <string>

#include <one/game/log.h>

namespace game {

Game::Game(unsigned int port)
    : _server(port)
    , _status(OneServerWrapper::StatusCode::starting)
    , _soft_stop_call_count(0)
    , _allocated_call_count(0)
    , _meta_data_call_count(0)
    , _host_information_call_count(0)
    , _application_instance_information_call_count(0)
    , _application_instance_get_status_call_count(0)
    , _application_instance_set_status_call_count(0) {}

Game::~Game() {
    _server.shutdown();
}

bool Game::init(int max_players, const std::string &name, const std::string &map,
                const std::string &mode, const std::string &version) {
    if (!_server.init()) {
        L_ERROR("failed to init server");
        return false;
    }

    OneServerWrapper::GameState state;
    state.players = 0;  // Game starts with 0 active players.
    state.max_players = max_players;
    state.name = name;
    state.map = map;
    state.mode = mode;
    state.version = version;
    _server.set_game_state(state);

    _server.set_soft_stop_callback(soft_stop_callback, this);
    _server.set_allocated_callback(allocated_callback, this);
    _server.set_meta_data_callback(meta_data_callback, this);
    _server.set_host_information_callback(host_information_callback, this);
    _server.set_application_instance_information_callback(
        application_instance_information_callback, this);
    _server.set_application_instance_get_status_callback(
        application_instance_get_status_callback, this);
    _server.set_application_instance_set_status_callback(
        application_instance_set_status_callback, this);

    return (_server.status() == OneServerWrapper::Status::waiting_for_client);
}

void Game::shutdown() {
    _server.shutdown();
}

void Game::alter_game_state() {
    // This is mainly to emulate a very simple game change (i.e.: changing both the number
    // of player & status.
    const auto &state = _server.game_state();
    auto new_state = state;

    if (state.max_players < state.players + 1) {
        new_state.players = 0;
    } else {
        new_state.players++;
    }

    _server.set_game_state(new_state);

    switch (_status) {
        case OneServerWrapper::StatusCode::starting:
            if (!_server.send_application_instance_set_status(_status)) {
                L_ERROR("failed to send set status code starting");
                break;
            }

            _status = OneServerWrapper::StatusCode::online;
            break;
        case OneServerWrapper::StatusCode::online:
            if (!_server.send_application_instance_set_status(_status)) {
                L_ERROR("failed to send set status code online");
                break;
            }

            _status = OneServerWrapper::StatusCode::allocated;
            break;
        case OneServerWrapper::StatusCode::allocated:
            if (!_server.send_application_instance_set_status(_status)) {
                L_ERROR("failed to send set status code allocated");
                break;
            }

            _status = OneServerWrapper::StatusCode::online;
            break;
        default:
            L_INFO("unhandled status: skipping set status");
    }

    if (!_server.send_application_instance_get_status()) {
        L_ERROR("failed to send get status code");
    }
}

void Game::update() {
    _server.update();
}

void Game::soft_stop_callback(int timeout, void *userdata) {
    L_INFO("soft stop called:");
    L_INFO("\ttimeout:" + std::to_string(timeout));
    auto game = reinterpret_cast<Game *>(userdata);
    if (game == nullptr) {
        L_ERROR("null userdata");
        return;
    }
    game->_soft_stop_call_count++;
}

void Game::allocated_callback(const OneServerWrapper::AllocatedData &data,
                              void *userdata) {
    L_INFO("allocated called:");
    L_INFO("\tmax_players:" + data.max_players);
    L_INFO("\tmap:" + data.map);
    auto game = reinterpret_cast<Game *>(userdata);
    if (game == nullptr) {
        L_ERROR("null userdata");
        return;
    }
    game->_allocated_call_count++;
}

void Game::meta_data_callback(const OneServerWrapper::MetaDataData &data,
                              void *userdata) {
    L_INFO("meta data called:");
    L_INFO("\tmap:" + data.map);
    L_INFO("\tmode:" + data.mode);
    L_INFO("\ttype:" + data.type);
    auto game = reinterpret_cast<Game *>(userdata);
    if (game == nullptr) {
        L_ERROR("null userdata");
        return;
    }
    game->_meta_data_call_count++;
}

void Game::host_information_callback(const OneServerWrapper::HostInformationData &data,
                                     void *) {
    L_INFO("host information called:");
    L_INFO("\tid:" + std::to_string(data.id));
    L_INFO("\tserver id:" + std::to_string(data.server_id));
    L_INFO("\tserver name:" + data.server_name);
}

void Game::application_instance_information_callback(
    const OneServerWrapper::ApplicationInstanceInformationData &data, void *) {
    L_INFO("application instance information called:");
    L_INFO("\tfleet id:" + data.fleet_id);
    L_INFO("\thost id:" + std::to_string(data.host_id));
    L_INFO("\tis virtual:" + std::to_string(data.is_virtual));
}

void Game::application_instance_get_status_callback(
    const OneServerWrapper::ApplicationInstanceGetStatusData &data, void *) {
    L_INFO("application instance get status called:");
    L_INFO("\tstatus:" + std::to_string(data.status));
}

void Game::application_instance_set_status_callback(
    const OneServerWrapper::ApplicationInstanceSetStatusData &data, void *) {
    L_INFO("application instance set status called:");
    L_INFO("\tcode:" + std::to_string(static_cast<int>(data.code)));
}

}  // namespace game
