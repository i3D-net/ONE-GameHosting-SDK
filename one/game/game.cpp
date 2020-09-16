#include <one/game/game.h>

#include <one/game/log.h>

#include <assert.h>
#include <string>

namespace game {

Game::Game(unsigned int port)
    : _server(port)
    , _soft_stop_call_count(0)
    , _allocated_call_count(0)
    , _meta_data_call_count(0) {}

Game::~Game() {
    _server.shutdown();
}

bool Game::init(int players, int max_players, const std::string &name,
                const std::string &map, const std::string &mode,
                const std::string &version) {
    if (!_server.init()) {
        L_ERROR("failed to init server");
        return false;
    }

    OneServerWrapper::GameState state;
    state.players = players;
    state.max_players = max_players;
    state.name = name;
    state.map = map;
    state.mode = mode;
    state.version = version;
    _server.set_game_state(state);

    _server.set_soft_stop_callback(soft_stop_callback, this);
    _server.set_allocated_callback(allocated_callback, this);
    _server.set_meta_data_callback(meta_data_callback, this);

    return (_server.status() == OneServerWrapper::Status::waiting_for_client);
}

void Game::shutdown() {
    _server.shutdown();
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

void Game::host_information_callback(OneServerWrapper::HostInformationData data) {
    L_INFO("host information called:");
    L_INFO("\tid:" + data.id);
    L_INFO("\tserver id:" + data.server_id);
    L_INFO("\tserver name:" + data.server_name);
}

void Game::application_instance_information_callback(
    OneServerWrapper::ApplicationInstanceInformationData data) {
    L_INFO("application instance information called:");
    L_INFO("\tfleet id:" + data.fleet_id);
    L_INFO("\thost id:" + data.host_id);
    L_INFO("\tis virtual:" + data.is_virtual);
}

void Game::application_instance_get_status_callback(
    OneServerWrapper::ApplicationInstanceGetStatusData data) {
    L_INFO("application instance get status called:");
    L_INFO("\tstatus:" + data.status);
}

void Game::application_instance_set_status_callback(
    OneServerWrapper::ApplicationInstanceSetStatusData data) {
    L_INFO("application instance set status called:");
    L_INFO("\tcode:" + data.code);
}

}  // namespace game
