#pragma once

#include <one/arcus/message.h>

#include <functional>
#include <string>

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

struct LifeStateRequest {};

struct LifeStateResponse {
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
int error_request(const Message &message, params::ErrorResponse &params);
int soft_stop_request(const Message &message, params::SoftStopRequest &params);
int allocated_request(const Message &message, params::AllocatedRequest &params);
int meta_data_request(const Message &message, params::MetaDataRequest &params);
int live_state_request(const Message &message, params::LifeStateRequest &params);
int live_state_response(const Message &message, params::LifeStateResponse &params);
int host_information_request(const Message &message, params::HostInformationRequest &params);
}  // namespace validation

namespace invocation {
int error_response(const Message &message, std::function<void(void *)> callback, void *data);
int soft_stop_request(const Message &message, std::function<void(void *, int)> callback,
                      void *data);
int allocated_request(const Message &message, std::function<void(void *, Array *)> callback,
                      void *data);
int meta_data_request(const Message &message, std::function<void(void *, Array *)> callback,
                      void *data);
int live_state_request(const Message &message, std::function<void(void *)> callback, void *data);
int live_state_response(
    const Message &message,
    std::function<void(void *, int, int, const std::string &, const std::string &,
                       const std::string &, const std::string &)>
        callback,
    void *data);
int host_informatio_request(const Message &message, std::function<void(void *)> callback,
                            void *data);
}  // namespace invocation

}  // namespace one
