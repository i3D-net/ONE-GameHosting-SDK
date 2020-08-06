#pragma once

#include <functional>
#include <string>

namespace one {

class Message;

namespace params {

struct SoftStop {
    int _timeout;
};

struct LifeStateRequest {};

struct LifeState {
    int _players;
    int _max_players;
    std::string _name;
    std::string _map;
    std::string _mode;
    std::string _version;

    // Need a clean way to pass the user defined fields.
};

}  // namespace params

namespace validate {
int soft_stop(const Message& message, params::SoftStop& params);
int live_state_request(const Message& message, params::LifeStateRequest& params);
int live_state(const Message& message, params::LifeState& params);
}  // namespace validate

namespace invoke {
int soft_stop(const Message& message, std::function<void(int)> callback);
int live_state_request(const Message& message, std::function<void()> callback);
}  // namespace invoke

}  // namespace one
