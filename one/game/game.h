#pragma once

#include <one/arcus/c_api.h>

namespace one {

class Game final {

public:
    Game(unsigned int port);
    ~Game();

    int init();
    int deinit();

    int tick();

protected:
    int update_number_players();
    int handle_updates();

private:
    int update();
    int status() const;
    int listen();
    int close();

    OneGameHostingApiPtr _api;
    OneServerPtr _server;

    const unsigned int _port;

    unsigned int _number_players;
};

}  // namespace one
