#include <one/game/game.h>

#include <assert.h>
#include <string>

#include <one/game/log.h>

namespace one_integration {

// Provides example global allocation hooks used to override the allocations
// made within the One Game Hosting SDK.
namespace allocation {

// For debub purposes.
size_t _alloc_count = 0;
size_t _free_count = 0;

size_t alloc_count() {
    return _alloc_count;
}

size_t free_count() {
    return _free_count;
}

// A custom memory alloc example.
void *alloc(size_t bytes) {
    _alloc_count++;
    return ::operator new(bytes);
}

// A custom memory free example.
void free(void *p) {
    _free_count++;
    ::operator delete(p);
}

}  // namespace allocation

Game::Game()
    : _one_server()  // The One Arcus Server's default constructor is called.
    , _soft_stop_receive_count(0)
    , _allocated_receive_count(0)
    , _metadata_receive_count(0)
    , _host_information_receive_count(0)
    , _application_instance_information_receive_count(0)
    , _quiet(false)
    , _players(0)
    , _max_players(0)
    , _matchmaking_status(MatchmakingStatus::none)
    , _previous_matchmaking_status(MatchmakingStatus::none) {}

Game::~Game() {
    _one_server.shutdown();
}

bool Game::init(unsigned int port, int max_players, const std::string &name,
                const std::string &map, const std::string &mode,
                const std::string &version) {
    const std::lock_guard<std::mutex> lock(_game);

    //----------------------------------------------------------------------
    // Init One Server and make it start listening for an incoming One Agent
    // client connection.

    if (!_one_server.init(allocation::alloc, allocation::free)) {
        L_ERROR("failed to init one server");
        return false;
    }

    if (!_one_server.listen(port)) {
        L_ERROR("failed to listen on one server");
        return false;
    }

    //------------------------------------------------------------
    // Set initial game state and register notification callbacks.

    _players = 0;  // Game starts with 0 active players.
    _max_players = max_players;
    _name = name;
    _map = map;
    _mode = mode;
    _version = version;

    update_arcus_server_game_state();

    _one_server.set_soft_stop_callback(soft_stop_callback, this);
    _one_server.set_allocated_callback(allocated_callback, this);
    _one_server.set_metadata_callback(metadata_callback, this);
    _one_server.set_host_information_callback(host_information_callback, this);
    _one_server.set_application_instance_information_callback(
        application_instance_information_callback, this);

    return (_one_server.status() == OneServerWrapper::Status::waiting_for_client);
}

void Game::shutdown() {
    const std::lock_guard<std::mutex> lock(_game);
    _one_server.shutdown();
}

void Game::alter_game_state() {
    const std::lock_guard<std::mutex> lock(_game);

    // The server will only be able to send game message and react to the agent message
    // only after it is ready. Trying to send a message while an agent is not connected
    // and not ready will return an error.
    if (_one_server.status() != OneServerWrapper::Status::ready) {
        return;
    }

    //--------------------------------------------------------------------------
    // Emulate a very simple game change. In a real integration the game would
    // need to update its game state from its game mode, player and match
    // systems.

    // The number of player is arbitrarily changed to trigger player joined and
    // left messages.
    if (_max_players < _players + 1) {
        _players = 0;
    } else {
        _players += 1;
    }

    update_arcus_server_game_state();
    const bool new_matchmaking_status =
        (_matchmaking_status != _previous_matchmaking_status);
    _previous_matchmaking_status = _matchmaking_status;

    //--------------------------------------------------------------------------
    // Update game server status. The game server must notify One when it is
    // finished initializing and ready to matchmake (online status). It may
    // also need to transition to and from the allocated status in response to
    // an allocated message from One.
    // Below, states are transitioned in the correct order but not based on any
    // real matchmaking as this is not a real game server.

    switch (_matchmaking_status) {
        case MatchmakingStatus::none:
            if (!_quiet) {
                L_INFO("application instance status none");
            }
            _matchmaking_status = MatchmakingStatus::starting;
            _one_server.set_application_instance_status(
                OneServerWrapper::ApplicationInstanceStatus::starting);
            break;
        case MatchmakingStatus::starting:
            if (!_quiet && new_matchmaking_status) {
                L_INFO("application instance status starting");
            }
            _matchmaking_status = MatchmakingStatus::online;
            _one_server.set_application_instance_status(
                OneServerWrapper::ApplicationInstanceStatus::online);
            break;
        case MatchmakingStatus::allocated:
            if (!_quiet && new_matchmaking_status) {
                L_INFO("application instance status allocated");
            }
            // Fake ending an allocated match and going back to online.
            if (_players == 0) {
                _matchmaking_status = MatchmakingStatus::online;
                _one_server.set_application_instance_status(
                    OneServerWrapper::ApplicationInstanceStatus::online);
            }
            break;
        case MatchmakingStatus::online:
            if (!_quiet && new_matchmaking_status) {
                L_INFO("application instance status online");
            }
            break;
        default:
            L_ERROR("invalid matchmaking status");
    }
}

void Game::update() {
    const std::lock_guard<std::mutex> lock(_game);
    _one_server.update(_quiet);
}

void Game::update_arcus_server_game_state() {
    OneServerWrapper::GameState game_state;

    game_state.players = _players;
    game_state.max_players = _max_players;
    game_state.name = _name;
    game_state.map = _map;
    game_state.mode = _mode;
    game_state.version = _version;

    _one_server.set_game_state(game_state);
}

void Game::set_player_count(int count) {
    if (count == _players) return;
    _players = count;
    update_arcus_server_game_state();
}

int Game::soft_stop_receive_count() const {
    const std::lock_guard<std::mutex> lock(_game);
    return _soft_stop_receive_count;
}

int Game::allocated_receive_count() const {
    const std::lock_guard<std::mutex> lock(_game);
    return _allocated_receive_count;
}

int Game::metadata_receive_count() const {
    const std::lock_guard<std::mutex> lock(_game);
    return _metadata_receive_count;
}

int Game::host_information_receive_count() const {
    const std::lock_guard<std::mutex> lock(_game);
    return _host_information_receive_count;
}

int Game::application_instance_information_receive_count() const {
    const std::lock_guard<std::mutex> lock(_game);
    return _application_instance_information_receive_count;
}

void Game::soft_stop_callback(int timeout, void *userdata) {
    auto game = reinterpret_cast<Game *>(userdata);
    if (game == nullptr) {
        L_ERROR("null userdata");
        return;
    }

    // A real game would schedule a graceful process shutdown here.
    if (!game->is_quiet()) {
        L_INFO("soft stop called:");
        L_INFO("\ttimeout:" + std::to_string(timeout));
    }

    game->_soft_stop_receive_count++;
}

void Game::allocated_callback(const OneServerWrapper::AllocatedData &data,
                              void *userdata) {
    auto game = reinterpret_cast<Game *>(userdata);
    if (game == nullptr) {
        L_ERROR("null userdata");
        return;
    }

    // A real game would use the given matchmaking here to host a match and
    // set its Application Instance Status to allocated when ready to accept
    // players.
    if (!game->is_quiet()) {
        L_INFO("allocated called:");
        L_INFO("\tmax_players:" + data.max_players);
        L_INFO("\tmap:" + data.map);
    }

    game->_allocated_receive_count++;

    game->_matchmaking_status = MatchmakingStatus::allocated;
    game->_one_server.set_application_instance_status(
        OneServerWrapper::ApplicationInstanceStatus::allocated);
}

void Game::metadata_callback(const OneServerWrapper::MetaDataData &data, void *userdata) {
    auto game = reinterpret_cast<Game *>(userdata);
    if (game == nullptr) {
        L_ERROR("null game");
        return;
    }
    // A real game would apply the metadata here to the server.
    if (!game->is_quiet()) {
        L_INFO("meta data called:");
        L_INFO("\tmap:" + data.map);
        L_INFO("\tmode:" + data.mode);
        L_INFO("\ttype:" + data.type);
    }

    game->_metadata_receive_count++;
}

void Game::host_information_callback(const OneServerWrapper::HostInformationData &data,
                                     void *userdata) {
    auto game = reinterpret_cast<Game *>(userdata);
    if (game == nullptr) {
        L_ERROR("null game");
        return;
    }

    // A real game can read host fields here, for example to post server name
    // information in the UI for the player to see.
    if (!game->is_quiet()) {
        L_INFO("host information called:");
        L_INFO("\tid:" + std::to_string(data.id));
        L_INFO("\tserver id:" + std::to_string(data.server_id));
        L_INFO("\tserver name:" + data.server_name);
    }

    game->_host_information_receive_count++;
}

void Game::application_instance_information_callback(
    const OneServerWrapper::ApplicationInstanceInformationData &data, void *userdata) {
    auto game = reinterpret_cast<Game *>(userdata);
    if (game == nullptr) {
        L_ERROR("null game");
        return;
    }

    // A real game could use information here for debugging or other purposes.
    if (!game->is_quiet()) {
        L_INFO("application instance information called:");
        L_INFO("\tfleet id:" + data.fleet_id);
        L_INFO("\thost id:" + std::to_string(data.host_id));
        L_INFO("\tis virtual:" + std::to_string(data.is_virtual));
    }

    game->_application_instance_information_receive_count++;
}

}  // namespace one_integration
