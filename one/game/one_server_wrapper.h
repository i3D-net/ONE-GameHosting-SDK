#pragma once

#include <one/arcus/c_api.h>

#include <string>

namespace one {

class OneServerWrapper final {

public:
    OneServerWrapper(unsigned int port, int queueLength);
    OneServerWrapper(const OneServerWrapper &) = delete;
    OneServerWrapper &operator=(const OneServerWrapper &) = delete;
    ~OneServerWrapper();

    int init(size_t max_message_in, size_t max_message_out);
    int shutdown();

    int update();
    int status() const;

    int send_error_response();
    int send_live_state_response(int player, int max_player, const std::string &name,
                                 const std::string &map, const std::string &mode,
                                 const std::string &version);
    int send_host_information_request();

    int set_soft_stop_request_callback(void (*callback)(void *data, int timeout), void *data);
    int set_allocated_request_callback(void (*callback)(void *data, void *), void *data);
    int set_meta_data_request_callback(void (*callback)(void *data, void *), void *data);
    int set_live_state_request_callback(void (*callback)(void *data), void *data);

    int prepare_error_response(OneMessagePtr message);
    int prepare_live_state_response(int player, int max_player, const char *name, const char *map,
                                    const char *mode, const char *version, OneMessagePtr message);
    int prepare_host_information_request(OneMessagePtr message);

private:
    int listen();

    OneGameHostingApiPtr _api;
    OneServerPtr _server;
    OneMessagePtr _error;
    OneMessagePtr _live_state;
    OneMessagePtr _host_information;

    const unsigned int _port;
    const int _queueLength;
};

}  // namespace one
