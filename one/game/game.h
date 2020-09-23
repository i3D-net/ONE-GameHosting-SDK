#pragma once

#include <one/game/one_server_wrapper.h>

#include <functional>
#include <string>

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

    void update();

    //--------------------------------------------------------------------------
    // Query how many times message game information has been requested from the
    // game by the One platform.

    int soft_stop_call_count() const {
        return _soft_stop_call_count;
    }
    int allocated_call_count() const {
        return _allocated_call_count;
    }
    int meta_data_call_count() const {
        return _meta_data_call_count;
    }

    // Exposed for testing purposes, however use of this should be kept to a
    // minimum or removed. The game, as much as reasonable, should be tested as
    // a black box.
    OneServerWrapper &one_server_wrapper() {
        return _server;
    }

private:
    static void soft_stop_callback(int timeout, void *userdata);
    static void allocated_callback(const OneServerWrapper::AllocatedData &data,
                                   void *userdata);
    static void meta_data_callback(const OneServerWrapper::MetaDataData &data,
                                   void *userdata);
    static void host_information_callback(OneServerWrapper::HostInformationData data);
    static void application_instance_information_callback(
        OneServerWrapper::ApplicationInstanceInformationData data);
    static void application_instance_get_status_callback(
        OneServerWrapper::ApplicationInstanceGetStatusData data);
    static void application_instance_set_status_callback(
        OneServerWrapper::ApplicationInstanceSetStatusData data);

    OneServerWrapper _server;

    int _soft_stop_call_count;
    int _allocated_call_count;
    int _meta_data_call_count;
};

}  // namespace game
