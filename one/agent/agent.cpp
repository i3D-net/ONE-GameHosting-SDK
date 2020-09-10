#include <one/agent/agent.h>

#include <one/arcus/message.h>

namespace i3d {
namespace one {

Error Agent::init(const char *addr, unsigned int port) {
    auto err = _client.init(addr, port);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::update() {
    auto err = _client.update();
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_soft_stop_request(int timeout) {
    auto err = _client.send_soft_stop_request(timeout);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_live_state_request() {
    auto err = _client.send_live_state_request();
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_allocated_request(Array *array) {
    auto err = _client.send_allocated_request(array);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_meta_data_request(Array *array) {
    auto err = _client.send_meta_data_request(array);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::set_error_response_callback(std::function<void(void *)> callback,
                                         void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    _client.set_error_callback(callback, data);
    return ONE_ERROR_NONE;
}

Error Agent::set_live_state_response_callback(
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    _client.set_live_state_callback(callback, data);
    return ONE_ERROR_NONE;
}

Error Agent::set_host_information_request_callback(std::function<void(void *)> callback,
                                                   void *data) {
    if (callback == nullptr) {
        return ONE_ERROR_VALIDATION_CALLBACK_IS_NULLPTR;
    }

    _client.set_host_information_request_callback(callback, data);
    return ONE_ERROR_NONE;
}

}  // namespace one
}  // namespace i3d