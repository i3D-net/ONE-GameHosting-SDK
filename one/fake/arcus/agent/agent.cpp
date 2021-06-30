#include <one/fake/arcus/agent/agent.h>

#include <stdlib.h>

#include <one/fake/arcus/agent/log.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>
#include <one/arcus/array.h>

namespace i3d {
namespace one {

Agent::Agent()
    : _quiet(false)
    , _live_state_receive_count(0)
    , _host_information_send_count(0)
    , _reverse_metadata_count(0)
    , _application_instance_information_send_count(0)
    , _application_instance_status_receive_count(0)
    , _custom_command_send_count(0) {}

OneError Agent::init(const char *addr, unsigned int port) {
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
            OStringStream stream_players;
            stream_players << "\tplayers:" << players;
            log_info(stream_players.str());
            OStringStream stream_max_players;
            stream_max_players << "\tmax_players:" << max_players;
            log_info(stream_max_players.str());
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
            log_info(stream.str());
        },
        nullptr);
    if (is_error(err)) {
        return err;
    }

    err = _client.set_reverse_metadata_callback(
        [this](void *, Array *data) {
            ++_reverse_metadata_count;
            if (_quiet) {
                return;
            }
            log_info("reverse metadata request received:");
            Object map;
            data->val_object(0, map);
            String map_name;
            map.val_string("value", map_name);
            Object mode;
            data->val_object(1, mode);
            String mode_name;
            mode.val_string("value", mode_name);
            Object type;
            data->val_object(2, type);
            String type_name;
            type.val_string("value", type_name);
            log_info("\tmap:" + map_name);
            log_info("\tmode:" + mode_name);
            log_info("\ttype:" + type_name);
        },
        nullptr);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError Agent::send_host_information() {
    // See
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#host-information
    // for more details.
    Object object;

    // For example mirroring the host information example payload.
    // Only a small subset of the available key/value pairs are used for demonstration.
    object.set_val_int("id", 123456);
    object.set_val_int("serverId", 8989);
    object.set_val_string("serverName", "server name 123456");
    // ...

    auto err = _client.send_host_information(object);
    if (is_error(err)) return err;
    _host_information_send_count++;
    return ONE_ERROR_NONE;
}

OneError Agent::send_application_instance_information() {
    // See
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#applicationinstance-information
    // for more details.
    Object object;

    // For example mirroring the applicationinstance information example payload.
    // Only a small subset of the available key/value pairs are used for demonstration.
    object.set_val_string("fleetId", "fleet_9876");
    object.set_val_int("hostId", 87654);
    object.set_val_bool("isVirtual", false);  // True if the host is a virtual machine.
    // ...

    auto err = _client.send_application_instance_information(object);
    if (is_error(err)) return err;
    _application_instance_information_send_count++;
    return ONE_ERROR_NONE;
}

OneError Agent::update() {
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

OneError Agent::send_soft_stop(int timeout) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.send_soft_stop(timeout);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError Agent::send_allocated(Array &array) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.send_allocated(array);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError Agent::send_metadata(Array &array) {
    const std::lock_guard<std::mutex> lock(_agent);
    auto err = _client.send_metadata(array);
    if (is_error(err)) {
        return err;
    }

    return ONE_ERROR_NONE;
}

OneError Agent::send_custom_command(Array &array) {
    // See
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#custom-command
    // for more details.

    OneError err = _client.send_custom_command(array);
    if (is_error(err)) return err;
    _custom_command_send_count++;
    return ONE_ERROR_NONE;
}

OneError Agent::set_live_state_callback(
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

OneError Agent::set_application_instance_status_callback(
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
