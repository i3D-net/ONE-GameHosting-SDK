#include <one/agent/agent.h>

#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/game/log.h>

namespace i3d {
namespace one {

Agent::Agent()
    : _quiet(false)
    , _live_state_receive_count(0)
    , _host_information_send_count(0)
    , _application_instance_information_send_count(0)
    , _application_instance_status_receive_count(0) {}

Error Agent::init(const char *addr, unsigned int port) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.init(addr, port);
    if (is_error(err)) {
        return err;
    }

    err = _client.set_live_state_callback(
        [this](void *, int players, int max_players, const std::string &name,
               const std::string &map, const std::string &mode,
               const std::string &version) {
            ++_live_state_receive_count;
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

    err = _client.set_application_instance_status_callback(
        [this](void *, int status) {
            ++_application_instance_status_receive_count;
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

Error Agent::send_host_information() {
    // Todo: set fake agent host information.
    Object object;
    auto err = _client.send_host_information(object);
    if (is_error(err)) return err;
    _host_information_send_count++;
    return ONE_ERROR_NONE;
}

Error Agent::send_application_instance_information() {
    // Todo: set fake info.
    Object object;
    auto err = _client.send_application_instance_information(object);
    if (is_error(err)) return err;
    _application_instance_information_send_count++;
    return ONE_ERROR_NONE;
}

Error Agent::update() {
    const std::lock_guard<std::mutex> lock(_agent);
    const bool was_ready = _client.status() == Client::Status::ready;
    auto err = _client.update();
    if (is_error(err)) {
        return err;
    }

    // Send agent information whenever the connection reaches a ready state.
    if (_client.status() == Client::Status::ready && !was_ready) {
        send_host_information();
        send_application_instance_information();
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

Error Agent::send_allocated(Array &array) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.send_allocated(array);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_metadata(Array &array) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.send_metadata(array);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::set_live_state_callback(
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.set_live_state_callback(callback, data);
    if (is_error(err)) {
        return err;
    }
    return ONE_ERROR_NONE;
}

Error Agent::set_application_instance_status_callback(
    std::function<void(void *, int)> callback, void *data) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.set_application_instance_status_callback(callback, data);
    if (is_error(err)) {
        return err;
    }
    return ONE_ERROR_NONE;
}

}  // namespace one
}  // namespace i3d
