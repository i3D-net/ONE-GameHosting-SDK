#pragma once

#include <one/arcus/c_api.h>

#include <functional>
#include <string>

namespace one {

class Game final {

public:
    Game(unsigned int port, int queueLength, int _players, int _max_players,
         const std::string &name, const std::string &map, const std::string &mode,
         const std::string &version);
    ~Game();

    int init(size_t max_message_in, size_t max_message_out);
    int shutdown();

    int update();
    int status() const;

    int send_error_response();

    int set_live_state_request_callback();
    static void send_live_state_callback(void *data);

    int send_host_information_request();

    int set_soft_stop_request_callback(void (*callback)(void *data, int timeout), void *data);
    int set_allocated_request_callback(void (*callback)(void *data, void *), void *data);
    int set_meta_data_request_callback(void (*callback)(void *data, void *), void *data);

private:
    int listen();

    OneGameHostingApiPtr _api;
    OneServerPtr _server;

    const unsigned int _port;
    const int _queueLength;

    int _players;
    int _max_players;
    std::string _name;
    std::string _map;
    std::string _mode;
    std::string _version;
};

}  // namespace one
