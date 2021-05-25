#include <one/fake/arcus/game/game.h>

#include <assert.h>
#include <ctime>
#include <string>
#include <sstream>

#include <one/fake/arcus/game/log.h>

namespace one_integration {

// Provides example global allocation hooks used to override the allocations
// made within the ONE Game Hosting SDK.
namespace allocation {

// For debug purposes.
size_t _alloc_count = 0;  // Including calls to realloc with a null ptr.
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
    return std::malloc(bytes);
}

// A custom memory free example.
void free(void *p) {
    // For counting purposes, exclude calls to deleting a nullptr. Passing it on
    // anyway to delete just as an example that it is safe to do so, although
    // there is no need to.
    if (p != nullptr) {
        _free_count++;
    }
    std::free(p);
}

void *realloc(void *p, size_t bytes) {
    if (p == nullptr) {
        _alloc_count++;
    }
    return std::realloc(p, bytes);
}

}  // namespace allocation

Game::Game()
    : _one_server()  // The One Arcus Server's default constructor is called.
    , _game()
    , _soft_stop_receive_count(0)
    , _allocated_receive_count(0)
    , _metadata_receive_count(0)
    , _host_information_receive_count(0)
    , _application_instance_information_receive_count(0)
    , _quiet(false)
    , _players(0)
    , _name()
    , _map()
    , _mode()
    , _version()
    , _exit_time(steady_clock::duration::zero())
    , _is_exit_time_enabled(false)
    , _matchmaking_status(MatchmakingStatus::none)
    , _previous_matchmaking_status(MatchmakingStatus::none)
    , _transition_delay(0)
    , _started_time(steady_clock::duration::zero())
    , _max_players(0)
    , _match_duration(0)
    , _match_start_time(steady_clock::duration::zero())
    , _match_status(MatchStatus::none) {}

Game::~Game() {
    _one_server.shutdown();
}

bool Game::init(unsigned int port, int max_players, const std::string &name,
                const std::string &map, const std::string &mode,
                const std::string &version, seconds delay) {
    const std::lock_guard<std::mutex> lock(_game);

    std::srand(std::time(nullptr));

    //----------------------------------------------------------------------
    // Init One Server so it can start listening for an incoming One Agent
    // client connection.

    OneServerWrapper::AllocationHooks hooks(allocation::alloc, allocation::free,
                                            allocation::realloc);
    if (!_one_server.init(port, hooks)) {
        L_ERROR("failed to init one server");
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

    _transition_delay = delay;

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

    update_startup();
    update_match();
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
            _one_server.set_application_instance_status(
                OneServerWrapper::ApplicationInstanceStatus::starting);
            break;
        case MatchmakingStatus::starting:
            if (!_quiet && new_matchmaking_status) {
                L_INFO("application instance status starting");
            }
            _one_server.set_application_instance_status(
                OneServerWrapper::ApplicationInstanceStatus::online);
            break;
        case MatchmakingStatus::online:
            if (!_quiet && new_matchmaking_status) {
                L_INFO("application instance status online");
            }
            _one_server.set_application_instance_status(
                OneServerWrapper::ApplicationInstanceStatus::online);
            break;
        case MatchmakingStatus::allocated:
            if (!_quiet && new_matchmaking_status) {
                L_INFO("application instance status allocated");
            }
            _one_server.set_application_instance_status(
                OneServerWrapper::ApplicationInstanceStatus::allocated);
            break;
        default:
            L_ERROR("invalid matchmaking status");
    }
}

void Game::update() {
    const std::lock_guard<std::mutex> lock(_game);

    if (_is_exit_time_enabled) {
        const auto time_zero = steady_clock::time_point(steady_clock::duration::zero());
        if (_exit_time != time_zero && steady_clock::now() > _exit_time) {
            L_INFO("ending process as a delayed response to a soft stop request");
            exit(0);
        }
    }

    _one_server.update(_quiet);
}

void Game::update_startup() {
    switch (_matchmaking_status) {
        case MatchmakingStatus::none:
            L_INFO("matchmaking status: starting");
            _matchmaking_status = MatchmakingStatus::starting;
            _started_time = steady_clock::now();
            return;
        case MatchmakingStatus::starting:
            if (_transition_delay < steady_clock::now() - _started_time) {
                L_INFO("matchamking status: online");
                _matchmaking_status = MatchmakingStatus::online;
            }
            return;
        case MatchmakingStatus::online:
        case MatchmakingStatus::allocated:
            return;
        default:
            L_ERROR("invalid matchmaking status");
    }
}

void Game::update_match() {
    switch (_match_status) {
        case MatchStatus::none:
            return;
        case MatchStatus::joining:
            if (_players == _max_players) {
                L_INFO("maximum number of player reached");
                _match_start_time = steady_clock::now();
                set_match_status(MatchStatus::playing);
                return;
            }

            ++_players;
            return;
        case MatchStatus::playing:
            if (_match_duration < steady_clock::now() - _match_start_time) {
                L_INFO("match duration has elapsed");
                set_match_status(MatchStatus::leaving);
            }
            return;
        case MatchStatus::leaving:
            if (_players == 0) {
                L_INFO("all player have left.");
                set_match_status(MatchStatus::none);
                _matchmaking_status = MatchmakingStatus::online;
                return;
            }

            --_players;
            return;
        default:
            return;
    }
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

void Game::set_match_status(MatchStatus status) {
    switch (status) {
        case MatchStatus::none:
            _players = 0;
            _max_players = 0;
            _match_duration = seconds(0);
            break;
        default:
            break;
    }

    _match_status = status;
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

void Game::set_player_count(int count) {
    if (count == _players) return;
    _players = count;
    update_arcus_server_game_state();
}

void Game::soft_stop_callback(int timeout, void *userdata) {
    auto game = reinterpret_cast<Game *>(userdata);
    if (game == nullptr) {
        L_ERROR("null userdata");
        return;
    }

    // A real game would schedule a graceful process shutdown here.
    if (!game->is_quiet()) {
        L_INFO("soft stop called");
        L_INFO("\ttimeout:" + std::to_string(timeout));
    }

    if (game->_is_exit_time_enabled) {
        auto delay_seconds = std::rand() % (2 * timeout);
        game->_exit_time = steady_clock::now() + seconds(delay_seconds);
        std::ostringstream stream;
        stream << "will shut down process in seconds: " << delay_seconds;
        L_INFO(stream.str().c_str());
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

    switch (game->_matchmaking_status) {
        case MatchmakingStatus::online:
            L_INFO("game is ready to process allocated messsage.");
            break;
        case MatchmakingStatus::allocated:
            L_INFO("game is already allocated: skipping allocated message.");
            return;
        case MatchmakingStatus::none:
        case MatchmakingStatus::starting:
        default:
            L_INFO("game is not ready to process allocated: skipping allocated message.");
            return;
    }

    // A real game would use the given matchmaking here to host a match and
    // set its Application Instance Status to allocated when ready to accept
    // players.
    if (!game->is_quiet()) {
        L_INFO("allocated called:");
        std::ostringstream stream;
        stream << "\tplayers:" << data.players;
        L_INFO(stream.str().c_str());
        stream.clear();
        stream << "\tduration:" << data.duration;
        L_INFO(stream.str().c_str());
    }

    game->_allocated_receive_count++;

    game->_max_players = data.players;
    game->_match_duration = seconds(data.duration);
    game->set_match_status(MatchStatus::joining);

    game->_matchmaking_status = MatchmakingStatus::allocated;
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
