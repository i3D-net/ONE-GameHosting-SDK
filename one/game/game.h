#pragma once

#include <functional>
#include <string>

#include <one/game/one_server_wrapper.h>

namespace game {

class OneServerWrapper;

class Game final {
public:
    Game(unsigned int port);
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    ~Game();

    bool init(int players, int max_players, const std::string &name,
              const std::string &map, const std::string &mode,
              const std::string &version);
    void shutdown();

    void update();

    // Exposed for testing purposes.
    OneServerWrapper &one_server_wrapper() {
        return _server;
    }

private:
    static void soft_stop_callback(int timeout);
    static void allocated_callback(OneServerWrapper::AllocatedData data);
    static void meta_data_callback(OneServerWrapper::MetaDataData data);
    static void host_information_callback(OneServerWrapper::HostInformationData data);
    static void application_instance_information_callback(
        OneServerWrapper::ApplicationInstanceInformationData data);
    static void application_instance_get_status_callback(
        OneServerWrapper::ApplicationInstanceGetStatusData data);
    static void application_instance_set_status_callback(
        OneServerWrapper::ApplicationInstanceSetStatusData data);

    OneServerWrapper _server;
};

}  // namespace game
