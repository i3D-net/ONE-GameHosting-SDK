#include <one/game/game.h>

#include <one/game/log.h>

#include <assert.h>
#include <string>

namespace game {

Game::Game(unsigned int port) : _server(port) {}

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

    OneServerWrapper::GameState state = {0};
    state.players = players;
    state.max_players = max_players;
    state.name = name;
    state.map = map;
    state.mode = mode;
    state.version = version;
    _server.set_game_state(state);

    _server.set_soft_stop_callback(soft_stop_callback);
    _server.set_allocated_callback(allocated_callback);
    _server.set_meta_data_callback(meta_data_callback);

    return (_server.status() == OneServerWrapper::Status::waiting_for_client);
}

void Game::shutdown() {
    _server.shutdown();
}

void Game::update() {
    _server.update();
}

void Game::soft_stop_callback(int timeout) {
    L_INFO("soft stop payload:");
    L_INFO("timeout:" + std::to_string(timeout));
}

void Game::allocated_callback(OneServerWrapper::AllocatedData data) {
    L_INFO("allocated payload:");
    L_INFO("max_players:" + data.max_players);
    L_INFO("map:" + data.map);
}

void Game::meta_data_callback(OneServerWrapper::MetaDataData data) {
    L_INFO("allocated payload:");
    L_INFO("map:" + data.map);
    L_INFO("mode:" + data.mode);
    L_INFO("type:" + data.type);
}

void Game::host_information_callback(OneServerWrapper::HostInformationData data)
{
    L_INFO("host information payload:");
    L_INFO("id:" + data.id);
    L_INFO("server id:" + data.server_id);
    L_INFO("server name:" + data.server_name);
}

void Game::application_instance_information_callback(OneServerWrapper::ApplicationInstanceInformationData data)
{
    L_INFO("application instance information payload:");
    L_INFO("fleet id:" + data.fleet_id);
    L_INFO("host id:" + data.host_id);
    L_INFO("is virtual:" + data.is_virtual);
}

void Game::application_instance_get_status_callback(OneServerWrapper::ApplicationInstanceGetStatusData data)
{
    L_INFO("application instance get status payload:");
    L_INFO("status:" + data.status);
}

void Game::application_instance_set_status_callback(OneServerWrapper::ApplicationInstanceSetStatusData data)
{
    L_INFO("application instance set status payload:");
    L_INFO("code:" + data.code);
}

}  // namespace game
