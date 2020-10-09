#pragma once

#include <one/game/one_server_wrapper.h>

#include <functional>
#include <string>
#include <mutex>

namespace one_integration {

/// A fake Game server. It owns a OneServerWrapper member that encapsulates
/// the c_api.h of the arcus server.
///
/// The Fake Game's purpose is to:
/// - Facilitate automated and manual integration testing of the SDK
/// - Illustrate an integration
///
/// Please note that this particular class is a completely fake game, and its
/// interface is not meant to represent those found in real games. Its public
/// surface's main goal is to assist integration-style testing. See the use of
/// its _one_server member variable for integration functionality.
class Game final {
public:
    Game();
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    ~Game();

    //------------
    // Life cycle.

    bool init(unsigned int port, int max_players, const std::string &name,
              const std::string &map, const std::string &mode,
              const std::string &version);
    void shutdown();

    void alter_game_state();

    void update();

    //--------------------------------------------------------------------------
    // Query how many times message game information has been requested from the
    // game by the One platform.

    int soft_stop_receive_count() const;
    int allocated_receive_count() const;
    int metadata_receive_count() const;
    int host_information_receive_count() const;
    int application_instance_information_receive_count() const;

    // Exposed for testing purposes, however use of this should be kept to a
    // minimum or removed. The game, as much as reasonable, should be tested as
    // a black box.
    OneServerWrapper &one_server_wrapper() {
        return _one_server;
    }

    // Exposed for testing purposes to avoid spamming std::error when testing for expected
    // failures.
    void set_quiet(bool quiet) {
        _quiet = quiet;
    }

    int player_count() const {
        return _players;
    }
    void set_player_count(int count);

private:
    static void soft_stop_callback(int timeout, void *userdata);
    static void allocated_callback(const OneServerWrapper::AllocatedData &data,
                                   void *userdata);
    static void metadata_callback(const OneServerWrapper::MetaDataData &data,
                                  void *userdata);
    static void host_information_callback(
        const OneServerWrapper::HostInformationData &data, void *userdata);
    static void application_instance_information_callback(
        const OneServerWrapper::ApplicationInstanceInformationData &data, void *userdata);

    void update_arcus_server_game_state();

    OneServerWrapper _one_server;

    int _soft_stop_receive_count;
    int _allocated_receive_count;
    int _metadata_receive_count;
    int _application_instance_information_receive_count;
    int _host_information_receive_count;

    bool _quiet;

    // Fake game state.
    int _players;
    int _max_players;
    std::string _name;
    std::string _map;
    std::string _mode;
    std::string _version;

    enum class MatchmakingStatus { none, starting, online, allocated };
    MatchmakingStatus _matchmaking_status;

    mutable std::mutex _game;
};

}  // namespace one_integration
