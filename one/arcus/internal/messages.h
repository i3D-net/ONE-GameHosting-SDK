#pragma once

#include <one/arcus/array.h>
#include <one/arcus/error.h>
#include <one/arcus/message.h>
#include <one/arcus/object.h>

#include <functional>

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
    String _name;
    String _map;
    String _mode;
    String _version;

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
Error host_information(const Message &message,
                                params::HostInformationResponse &params);
Error application_instance_information(
    const Message &message, params::ApplicationInstanceInformationResponse &params);
Error application_instance_status(
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
    std::function<void(void *, int, int, const String &, const String &,
                       const String &, const String &)>
        callback,
    void *data);

Error host_information(const Message &message,
                                std::function<void(void *, Object *)> callback,
                                void *data);

Error application_instance_information(
    const Message &message, std::function<void(void *, Object *)> callback, void *data);

Error application_instance_status(const Message &message,
                                              std::function<void(void *, int)> callback,
                                              void *data);

}  // namespace invocation

}  // namespace one
}  // namespace i3d
