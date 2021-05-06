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
OneError soft_stop(const Message &message, params::SoftStopRequest &params);
OneError allocated(const Message &message, params::AllocatedRequest &params);
OneError metadata(const Message &message, params::MetaDataRequest &params);
OneError live_state(const Message &message, params::LiveStateResponse &params);
OneError host_information(const Message &message,
                                params::HostInformationResponse &params);
OneError application_instance_information(
    const Message &message, params::ApplicationInstanceInformationResponse &params);
OneError application_instance_status(
    const Message &message, params::ApplicationInstanceSetStatusRequest &params);
}  // namespace validation

namespace invocation {

OneError soft_stop(const Message &message, std::function<void(void *, int)> callback,
                void *data);

OneError allocated(const Message &message, std::function<void(void *, Array *)> callback,
                void *data);

OneError metadata(const Message &message, std::function<void(void *, Array *)> callback,
               void *data);

OneError live_state(
    const Message &message,
    std::function<void(void *, int, int, const String &, const String &,
                       const String &, const String &)>
        callback,
    void *data);

OneError host_information(const Message &message,
                                std::function<void(void *, Object *)> callback,
                                void *data);

OneError application_instance_information(
    const Message &message, std::function<void(void *, Object *)> callback, void *data);

OneError application_instance_status(const Message &message,
                                              std::function<void(void *, int)> callback,
                                              void *data);

}  // namespace invocation

}  // namespace one
}  // namespace i3d
