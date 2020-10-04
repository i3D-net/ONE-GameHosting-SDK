#include <one/agent/agent.h>

#include <one/arcus/message.h>
#include <one/game/log.h>

namespace i3d {
namespace one {

Agent::Agent()
    : _quiet(false)
    , _live_state_call_count(0)
    , _host_information_call_count(0)
    , _application_instance_information_call_count(0)
    , _application_instance_get_status_call_count(0)
    , _application_instance_set_status_call_count(0) {}

Error Agent::init(const char *addr, unsigned int port) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.init(addr, port);
    if (is_error(err)) {
        return err;
    }

    err = _client.set_live_state_response_callback(
        [this](void *, int players, int max_players, const std::string &name,
               const std::string &map, const std::string &mode,
               const std::string &version) {
            ++_live_state_call_count;
            if (_quiet) {
                return;
            }
            L_INFO("live state event response received:");
            L_INFO("\tplayers:" + std::to_string(players));
            L_INFO("\tmax_players:" + std::to_string(max_players));
            L_INFO("\tname:" + name);
            L_INFO("\tmap:" + map);
            L_INFO("\tmode:" + mode);
            L_INFO("\tversion:" + version);
        },
        nullptr);
    if (is_error(err)) {
        return err;
    }

    err = _client.set_host_information_request_callback(
        [this](void *) {
            ++_host_information_call_count;
            if (_quiet) {
                return;
            }
            L_INFO("host information request received:");
        },
        nullptr);
    if (is_error(err)) {
        return err;
    }

    err = _client.set_application_instance_information_request_callback(
        [this](void *) {
            ++_application_instance_information_call_count;
            if (_quiet) {
                return;
            }
            L_INFO("application instance information request received:");
        },
        nullptr);
    if (is_error(err)) {
        return err;
    }

    err = _client.set_application_instance_get_status_request_callback(
        [this](void *) {
            ++_application_instance_get_status_call_count;
            if (_quiet) {
                return;
            }
            L_INFO("application instance get status request received:");
        },
        nullptr);
    if (is_error(err)) {
        return err;
    }

    err = _client.set_application_instance_set_status_request_callback(
        [this](void *, int status) {
            ++_application_instance_set_status_call_count;
            if (_quiet) {
                return;
            }
            L_INFO("application instance set status request received:");
            L_INFO("\tstatus:" + std::to_string(status));
        },
        nullptr);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::update() {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.update();
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_soft_stop(int timeout) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.send_soft_stop(timeout);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_live_state_request() {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.send_live_state_request();
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_allocated(Array *array) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.send_allocated(array);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_metadata(Array *array) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.send_metadata(array);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::set_live_state_response_callback(
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.set_live_state_response_callback(callback, data);
    if (is_error(err)) {
        return err;
    }
    return ONE_ERROR_NONE;
}

Error Agent::set_host_information_request_callback(std::function<void(void *)> callback,
                                                   void *data) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.set_host_information_request_callback(callback, data);
    if (is_error(err)) {
        return err;
    }
    return ONE_ERROR_NONE;
}

Error Agent::set_application_instance_information_request_callback(
    std::function<void(void *)> callback, void *data) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err =
        _client.set_application_instance_information_request_callback(callback, data);
    if (is_error(err)) {
        return err;
    }
    return ONE_ERROR_NONE;
}

Error Agent::set_application_instance_get_status_request_callback(
    std::function<void(void *)> callback, void *data) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err =
        _client.set_application_instance_get_status_request_callback(callback, data);
    if (is_error(err)) {
        return err;
    }
    return ONE_ERROR_NONE;
}

Error Agent::set_application_instance_set_status_request_callback(
    std::function<void(void *, int)> callback, void *data) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err =
        _client.set_application_instance_set_status_request_callback(callback, data);
    if (is_error(err)) {
        return err;
    }
    return ONE_ERROR_NONE;
}

}  // namespace one
}  // namespace i3d
