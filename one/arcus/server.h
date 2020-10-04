#pragma once

#include <one/arcus/error.h>

#include <functional>
#include <mutex>
#include <string>

namespace i3d {
namespace one {

class Array;
class Connection;
class Message;
class Object;
class Socket;

namespace callback {

struct ServerCallbacks {
    std::function<void(void *, int)> _soft_stop;
    void *_soft_stop_userdata;
    std::function<void(void *, Array *)> _allocated;
    void *_allocated_userdata;
    std::function<void(void *, Array *)> _metadata;
    void *_metadata_userdata;
    std::function<void(void *, Object *)> _host_information_response;
    void *_host_information_response_data;
    std::function<void(void *, Object *)> _application_instance_information_response;
    void *_application_instance_information_response_data;
    std::function<void(void *, int)> _application_instance_get_status_response;
    void *_application_instance_get_status_response_data;
    std::function<void(void *, int)> _application_instance_set_status_response;
    void *_application_instance_set_status_response_data;
};

}  // namespace callback

// An Arcus Server is designed for use by a Game. It allows an Arcus One Agent
// to connect and communicate with the game.
class Server final {
public:
    Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;
    ~Server();

    Error init();

    Error shutdown();

    // Note these MUST be kept in sync with the values in c_api.cpp, or
    // otherwise ensured to translate properly to the c-api values.
    enum class Status {
        uninitialized = 0,
        initialized,
        waiting_for_client,
        handshake,
        ready,
        error
    };
    Status status() const;
    static std::string status_to_string(Status status);

    Error listen(unsigned int port);

    // Process pending received and outgoing messages. Any incoming messages are
    // validated according to the Arcus API version standard, and callbacks, if
    // set, are called. Messages without callbacks set are dropped and ignored.
    Error update();

    //------------------------------------------------------------------------------
    // Callbacks to be notified of all possible incoming Arcus messages.

    // Todo: update functions to match complete list from One API v2.

    // set the callback for when a soft_stop message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    Error set_soft_stop_callback(std::function<void(void *, int)> callback, void *data);

    // set the callback for when a allocated message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    Error set_allocated_callback(std::function<void(void *, Array *)> callback,
                                 void *data);

    // set the callback for when a meta_data message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    Error set_meta_data_callback(std::function<void(void *, Array *)> callback,
                                 void *data);

    // set the callback for when a host_information_response message in received.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    Error set_host_information_response_callback(
        std::function<void(void *, Object *)> callback, void *data);

    // set the callback for when a application_instance_information_response message in
    // received. The `void *data` is the user provided & will be passed as the first
    // argument of the callback when invoked. The `data` can be nullptr, the callback is
    // responsible to use the data properly.
    Error set_application_instance_information_response_callback(
        std::function<void(void *, Object *)> callback, void *data);

    // set the callback for when a application_instance_get_status_response message in
    // received. The `void *data` is the user provided & will be passed as the first
    // argument of the callback when invoked. The `data` can be nullptr, the callback is
    // responsible to use the data properly.
    Error set_application_instance_get_status_response_callback(
        std::function<void(void *, int)> callback, void *data);

    // set the callback for when a application_instance_set_status_response message in
    // received. The `void *data` is the user provided & will be passed as the first
    // argument of the callback when invoked. The `data` can be nullptr, the callback is
    // responsible to use the data properly.
    Error set_application_instance_set_status_response_callback(
        std::function<void(void *, int)> callback, void *data);

    //------------------------------------------------------------------------------
    // Outgoing.

    // send live_state_response opcode message.
    // Message Mandatory Content:
    // {
    //   "players" : 0,
    //   "maxPlayers" : 0,
    //   "name" : "",
    //   "map" : "",
    //   "mode" : "",
    //   "version" : ""
    // }
    Error send_live_state_response(const Message &message);

    // send host_information_request.
    // Message Empty Content:
    // {}
    Error send_host_information_request(const Message &message);

    // send application_instance_information_request.
    // Message Empty Content:
    // {}
    Error send_application_instance_information_request(const Message &message);

    // send application_instance_get_status_request.
    // Message Empty Content:
    // {}
    Error send_application_instance_get_status_request(const Message &message);

    // send application_instance_set_status_request.
    // Message Empty Content:
    // {
    //   "status": 0
    // }
    Error send_application_instance_set_status_request(const Message &message);

private:
    bool is_initialized() const;
    Error update_listen_socket();

    Error process_incoming_message(const Message &message);
    // The server must have an active and ready listen connection in order to
    // send outgoing messages. If not, either ONE_ERROR_SERVER_CONNECTION_IS_NULLPTR or
    // ONE_ERROR_SERVER_CONNECTION_NOT_READY is returned and the message is
    // not sent.
    Error process_outgoing_message(const Message &message);

    Socket *_listen_socket;
    Socket *_client_socket;
    Connection *_client_connection;

    bool _is_waiting_for_client;

    callback::ServerCallbacks _callbacks;

    mutable std::mutex _server;
};

}  // namespace one
}  // namespace i3d
