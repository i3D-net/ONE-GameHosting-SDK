#include <one/agent/agent.h>

#include <one/arcus/message.h>

namespace one {

int Agent::init(const char *addr, unsigned int port) {
    int error = _client.init(addr, port);
    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::update() {
    int error = _client.update();
    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::send_soft_stop_request(int timeout) {
    int error = _client.send_soft_stop_request(timeout);
    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::send_live_state_request() {
    int error = _client.send_live_state_request();
    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::send_allocated_request(Array *array) {
    int error = _client.send_allocated_request(array);
    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::send_meta_data_request(Array *array) {
    int error = _client.send_meta_data_request(array);
    if (error != 0) {
        return error;
    }

    return 0;
}

int Agent::set_error_response_callback(std::function<void(void *)> callback, void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _client.set_error_callback(callback, data);
    return 0;
}

int Agent::set_live_state_response_callback(
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _client.set_live_state_callback(callback, data);
    return 0;
}

int Agent::set_host_information_request_callback(std::function<void(void *)> callback,
                                                 void *data) {
    if (callback == nullptr) {
        return -1;
    }

    _client.set_host_information_request_callback(callback, data);
    return 0;
}

}  // namespace one
