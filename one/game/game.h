#pragma once

#include <one/arcus/c_api.h>

#include <functional>
#include <string>

namespace one {

class Game final {

public:
    Game(unsigned int port, int _players, int _max_players, const std::string &name,
         const std::string &map, const std::string &mode, const std::string &version);
    ~Game();

    int init();
    int shutdown();

    int tick();

    int set_live_state_request_callback();
    static void send_live_state_callback(void *data);

private:
    int update();
    int status() const;
    int listen();

    OneGameHostingApiPtr _api;
    OneServerPtr _server;

    const unsigned int _port;

    int _players;
    int _max_players;
    std::string _name;
    std::string _map;
    std::string _mode;
    std::string _version;
};

}  // namespace one
