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

struct LiveStateResponse {
    int _players;
    int _max_players;
    std::string _name;
    std::string _map;
    std::string _mode;
    std::string _version;

    // Need a clean way to pass the user defined fields.
};

struct HostInformationResponse {
    Object _host_information;
};

struct ApplicationInstanceInformationResponse {
    Object _application_instance_information;
};

struct ApplicationInstanceSetStatusRequest {
    int _status;
};

}  // namespace params

namespace validation {
Error soft_stop(const Message &message, params::SoftStopRequest &params);
Error allocated(const Message &message, params::AllocatedRequest &params);
Error metadata(const Message &message, params::MetaDataRequest &params);
Error live_state(const Message &message, params::LiveStateResponse &params);
Error host_information_response(const Message &message,
                                params::HostInformationResponse &params);
Error application_instance_information_response(
    const Message &message, params::ApplicationInstanceInformationResponse &params);
Error application_instance_set_status_request(
    const Message &message, params::ApplicationInstanceSetStatusRequest &params);
}  // namespace validation

namespace invocation {

Error soft_stop(const Message &message, std::function<void(void *, int)> callback,
                void *data);

Error allocated(const Message &message, std::function<void(void *, Array *)> callback,
                void *data);

Error metadata(const Message &message, std::function<void(void *, Array *)> callback,
               void *data);

Error live_state(
    const Message &message,
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data);

Error host_information_response(const Message &message,
                                std::function<void(void *, Object *)> callback,
                                void *data);

Error application_instance_information_response(
    const Message &message, std::function<void(void *, Object *)> callback, void *data);

Error application_instance_set_status_request(const Message &message,
                                              std::function<void(void *, int)> callback,
                                              void *data);

}  // namespace invocation

}  // namespace one
}  // namespace i3d
