#pragma once

#include <one/arcus/array.h>
#include <one/arcus/message.h>
#include <one/arcus/error.h>

#include <functional>
#include <string>

namespace i3d {
namespace one {

namespace params {

struct ErrorResponse {};

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

struct HostInformationRequest {};

}  // namespace params

namespace validation {
Error error_response(const Message &message, params::ErrorResponse &params);
Error soft_stop_request(const Message &message, params::SoftStopRequest &params);
Error allocated_request(const Message &message, params::AllocatedRequest &params);
Error meta_data_request(const Message &message, params::MetaDataRequest &params);
Error live_state_request(const Message &message, params::LiveStateRequest &params);
Error live_state_response(const Message &message, params::LiveStateResponse &params);
Error host_information_request(const Message &message,
                               params::HostInformationRequest &params);
}  // namespace validation

namespace invocation {
Error error_response(const Message &message, std::function<void(void *)> callback,
                     void *data);
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
Error host_information_request(const Message &message,
                               std::function<void(void *)> callback, void *data);
}  // namespace invocation

}  // namespace one
}  // namespace i3d