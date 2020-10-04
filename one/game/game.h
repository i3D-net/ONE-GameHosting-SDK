#pragma once

#include <one/game/one_server_wrapper.h>

#include <functional>
#include <string>
#include <mutex>

namespace game {

///
/// A fake Game server. It owns a OneServerWrapper member that encapsulates
/// the c_api.h of the arcus server.
///
/// The Fake Game's purpose is to:
/// - Facilitate automated and manual integration testing of the SDK
/// - Illustrate an integration
///
class Game final {
public:
    Game(unsigned int port);
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    ~Game();

    //------------
    // Life cycle.

    bool init(int max_players, const std::string &name, const std::string &map,
              const std::string &mode, const std::string &version);
    void shutdown();

    void alter_game_state();

    void update();

    //--------------------------------------------------------------------------
    // Query how many times message game information has been requested from the
    // game by the One platform.

    int soft_stop_call_count() const;
    int allocated_call_count() const;
    int meta_data_call_count() const;
    int host_information_send_count() const;
    int application_instance_information_call_count() const;
    int application_instance_get_status_call_count() const;
    int application_instance_set_status_call_count() const;

    // Exposed for testing purposes, however use of this should be kept to a
    // minimum or removed. The game, as much as reasonable, should be tested as
    // a black box.
    OneServerWrapper &one_server_wrapper() {
        return _server;
    }

    // Exposed for testing purposes to avoid spamming std::error when testing for expected
    // failures.
    void set_quiet(bool quiet) {
        _quiet = quiet;
    }

private:
    static void soft_stop_callback(int timeout, void *userdata);
    static void allocated_callback(const OneServerWrapper::AllocatedData &data,
                                   void *userdata);
    static void meta_data_callback(const OneServerWrapper::MetaDataData &data,
                                   void *userdata);
    static void host_information_callback(
        const OneServerWrapper::HostInformationData &data, void *userdata);
    static void application_instance_information_callback(
        const OneServerWrapper::ApplicationInstanceInformationData &data, void *userdata);
    static void application_instance_get_status_callback(
        const OneServerWrapper::ApplicationInstanceGetStatusData &data, void *userdata);
    static void application_instance_set_status_callback(
        const OneServerWrapper::ApplicationInstanceSetStatusData &data, void *userdata);

    OneServerWrapper _server;

    int _soft_stop_call_count;
    int _allocated_call_count;
    int _meta_data_call_count;
    int _host_information_send_count;
    int _application_instance_information_call_count;
    int _application_instance_get_status_call_count;
    int _application_instance_set_status_call_count;

    bool _quiet;

    // This is to emulate a game internal mecanims to get the number of players, current
    // maps, etc...
    // It is willfully avoiding use of OneServerWrapper::GameState to show how to bridge
    // the gap between Game & OneServerWrapper.
    int _players;
    int _max_players;
    std::string _name;
    std::string _map;
    std::string _mode;
    std::string _version;
    bool _starting;
    bool _online;
    bool _allocated;

    mutable std::mutex _game;
};

}  // namespace game
