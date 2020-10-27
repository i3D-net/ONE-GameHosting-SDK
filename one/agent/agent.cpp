#include <one/agent/agent.h>

#include <stdlib.h>

#include <one/agent/log.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>

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
        [this](void *, int players, int max_players, const String &name,
               const String &map, const String &mode, const String &version) {
            ++_live_state_receive_count;
            if (_quiet) {
                return;
            }
            log_info("live state event response received:");
            OStringStream stream;
            stream << "\tplayers:" << players;
            log_info(stream.str());
            stream.clear();
            stream << "\tmax_players:" + max_players;
            log_info(stream.str());
            log_info("\tname:" + name);
            log_info("\tmap:" + map);
            log_info("\tmode:" + mode);
            log_info("\tversion:" + version);
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
            log_info("application instance set status request received:");
            OStringStream stream;
            stream << "\tstatus:" << status;
        },
        nullptr);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

Error Agent::send_host_information() {
    // see
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#host-information
    // for more details
    Object object;

    // For example mirroring the host information example payload
    // object.set_val_int("id", 0);
    // object.set_val_int("serverId", 0);
    // ...

    auto err = _client.send_host_information(object);
    if (is_error(err)) return err;
    _host_information_send_count++;
    return ONE_ERROR_NONE;
}

Error Agent::send_application_instance_information() {
    // see
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#applicationinstance-information
    // for more details
    Object object;

    // For example mirroring the applicationinstance information example payload
    // object.set_val_string("fleetId", "");
    // object.set_val_int("hostId", 0);
    // ...

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
    std::function<void(void *, int, int, const String &, const String &, const String &,
                       const String &)>
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
