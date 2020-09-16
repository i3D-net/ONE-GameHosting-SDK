#pragma once

#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>

#include <functional>
#include <string>

namespace i3d {
namespace one {

namespace params {

struct SoftStopRequest {
    int _timeout;
};

struct AllocatedRequest {
    Array _data;
};

struct MetaDataRequest {
    Array _data;
};

struct LiveStateRequest {};

struct LiveStateResponse {
    int _players;
    int _max_players;
    std::string _name;
    std::string _map;
    std::string _mode;
    std::string _version;

    // Need a clean way to pass the user defined fields.
};

struct PlayerJoinedEventResponse {
    int _num_players;
};

struct PlayerLeftResponse {
    int _num_players;
};

struct HostInformationRequest {};

struct HostInformationResponse {
    Object _host_information;
};

struct ApplicationInstanceInformationRequest {};

struct ApplicationInstanceInformationResponse {
    Object _application_instance_information;
};

struct ApplicationInstanceGetStatusRequest {};

struct ApplicationInstanceGetStatusResponse {
    int _status;
};

struct ApplicationInstanceSetStatusRequest {
    int _status;
};

struct ApplicationInstanceSetStatusResponse {
    int _code;
};

}  // namespace params

namespace validation {
Error soft_stop_request(const Message &message, params::SoftStopRequest &params);
Error allocated_request(const Message &message, params::AllocatedRequest &params);
Error meta_data_request(const Message &message, params::MetaDataRequest &params);
Error live_state_request(const Message &message, params::LiveStateRequest &params);
Error live_state_response(const Message &message, params::LiveStateResponse &params);
Error player_joined_event_response(const Message &message,
                                   params::PlayerJoinedEventResponse &params);
Error player_left_response(const Message &message, params::PlayerLeftResponse &params);
Error host_information_request(const Message &message,
                               params::HostInformationRequest &params);
Error host_information_response(const Message &message,
                                params::HostInformationResponse &params);
Error application_instance_information_request(
    const Message &message, params::ApplicationInstanceInformationRequest &params);
Error application_instance_information_response(
    const Message &message, params::ApplicationInstanceInformationResponse &params);
Error application_instance_get_status_request(
    const Message &message, params::ApplicationInstanceGetStatusRequest &params);
Error application_instance_get_status_response(
    const Message &message, params::ApplicationInstanceGetStatusResponse &params);
Error application_instance_set_status_request(
    const Message &message, params::ApplicationInstanceSetStatusRequest &params);
Error application_instance_set_status_response(
    const Message &message, params::ApplicationInstanceSetStatusResponse &params);
}  // namespace validation

namespace invocation {
Error soft_stop_request(const Message &message, std::function<void(void *, int)> callback,
                        void *data);
Error allocated_request(const Message &message,
                        std::function<void(void *, Array *)> callback, void *data);
Error meta_data_request(const Message &message,
                        std::function<void(void *, Array *)> callback, void *data);
Error live_state_request(const Message &message, std::function<void(void *)> callback,
                         void *data);
Error live_state_response(
    const Message &message,
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data);
Error player_joined_event_response(const Message &message,
                                   std::function<void(void *, int)> callback, void *data);
Error player_left_response(const Message &message,
                           std::function<void(void *, int)> callback, void *data);

Error host_information_request(const Message &message,
                               std::function<void(void *)> callback, void *data);
Error host_information_response(const Message &message,
                                std::function<void(void *, Object *)> callback,
                                void *data);

Error application_instance_information_request(const Message &message,
                                               std::function<void(void *)> callback,
                                               void *data);
Error application_instance_information_response(
    const Message &message, std::function<void(void *, Object *)> callback, void *data);

Error application_instance_get_status_request(const Message &message,
                                              std::function<void(void *)> callback,
                                              void *data);
Error application_instance_get_status_response(const Message &message,
                                               std::function<void(void *, int)> callback,
                                               void *data);
Error application_instance_set_status_request(const Message &message,
                                              std::function<void(void *, int)> callback,
                                              void *data);
Error application_instance_set_status_response(const Message &message,
                                               std::function<void(void *, int)> callback,
                                               void *data);
}  // namespace invocation

}  // namespace one
}  // namespace i3d
