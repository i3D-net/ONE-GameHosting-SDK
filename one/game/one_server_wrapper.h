#pragma once

#include <functional>
#include <string>

//----------------------------------------------
// One SDK object types forward declarations

struct OneServer;
typedef OneServer *OneServerPtr;

struct OneMessage;
typedef OneMessage *OneMessagePtr;

struct OneArray;
typedef OneArray *OneArrayPtr;

struct OneObject;
typedef OneObject *OneObjectPtr;

namespace game {

///
/// OneServerWrapper encapsulates the integration for the One Arcus Server and
/// provides a game interface that hides the One Arcus Server API implementation
/// from the game. Errors are handled directly in the implementation of the
/// wrapper.
///
class OneServerWrapper final {
public:
    OneServerWrapper(unsigned int port);
    OneServerWrapper(const OneServerWrapper &) = delete;
    OneServerWrapper &operator=(const OneServerWrapper &) = delete;
    ~OneServerWrapper();

    // Init creates the server and listens.
    bool init();

    //
    void shutdown();

    enum class Status {
        uninitialized = 0,
        initialized,
        waiting_for_client,
        handshake,
        ready,
        error,
        unknown
    };
    static std::string status_to_string(Status status);
    Status status() const;

    struct GameState {
        GameState() : players(0), max_players(0), name(), map(), mode(), version() {}

        int players;          // Game number of players.
        int max_players;      // Game max number of players.
        std::string name;     // Server name.
        std::string map;      // Game map.
        std::string mode;     // Game mode.
        std::string version;  // Game version.
    };
    // Set the game state to the current value. The wrapper uses this to send
    // the current state to the One Platform, when requested to do so.
    void set_game_state(const GameState &);

    // Must called often (e.g. each frame). Updates the Arcus Server, which
    // processes incoming and outgoing messages.
    void update();

    // Sets a callback that is triggered when the remote client has notified the
    // server that it must gracefully exit the entire process. A timeout in
    // seconds is passed to allow the server to wait for as much active gameplay
    // (e.g. an active match) as possible to finish before shutting down. If the
    // timeout expires before the process is closed, the process will be
    // terminated forcefully.
    // userdata may be nullptr.
    void set_soft_stop_callback(std::function<void(int timeout, void *userdata)> callback,
                                void *userdata);

    // The allocation request has a optional body containing a JSON object with key value
    // pairs for meta data this keys and values are definable by the customer.
    // The current values are matching the payload shown in the documentation at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#allocated-request
    struct AllocatedData {
        AllocatedData() : max_players(), map() {}

        std::string max_players;  // Game max number of players.
        std::string map;          // Game map.
    };
    void set_allocated_callback(
        std::function<void(const AllocatedData &, void *)> callback, void *userdata);

    // The meta data request has a optional body containing a JSON object with key value
    // pairs for meta data this keys and values are definable by the customer.
    // The current values are matching the payload shown in the documentation at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#meta-data-request
    struct MetaDataData {
        MetaDataData() : map(), mode(), type() {}

        std::string map;   // Game map.
        std::string mode;  // Game mode.
        std::string type;  // Game type.
    };
    void set_meta_data_callback(
        std::function<void(const MetaDataData &, void *)> callback, void *userdata);

    // The host information response has a payload as defined at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#host-information-response
    // In this example only a handfull of fields are used for simplicity.
    struct HostInformationData {
        HostInformationData() : id(0), server_id(0), server_name() {}

        int id;                   // id.
        int server_id;            // server id.
        std::string server_name;  // server name.
        // ... add members as needed.
    };
    void set_host_information_callback(std::function<void(HostInformationData)> callback);

    // The application instancate information response has a payload as defined at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#applicationinstance-information-response
    // In this example only a handfull of fields are used for simplicity.
    struct ApplicationInstanceInformationData {
        ApplicationInstanceInformationData() : fleet_id(), host_id(0), is_virtual(0) {}

        std::string fleet_id;  // fleet id.
        int host_id;           // host id.
        int is_virtual;        // is virtual.
        // ... add members as needed.
    };
    void set_application_instance_information_callback(
        std::function<void(ApplicationInstanceInformationData)> callback);

    // The application instancate information response has a payload as defined at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#applicationinstance-get-status-response
    // In this example only a handfull of fields are used for simplicity.
    struct ApplicationInstanceGetStatusData {
        ApplicationInstanceGetStatusData() : status(0) {}

        int status;  // status.
    };
    void set_application_instance_get_status_callback(
        std::function<void(ApplicationInstanceGetStatusData)> callback);

    // The application instancate information response has a payload as defined at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#applicationinstance-set-status-response
    // In this example only a handfull of fields are used for simplicity.
    // Todo: The intent is to use this wrapper as a complete integration
    // reference.
    // - all standard fields should be utilized here
    // - iterate on online api docs as they are not clear
    struct ApplicationInstanceSetStatusData {
        ApplicationInstanceSetStatusData() : code(0) {}

        int code;  // Todo: document.
    };
    void set_application_instance_set_status_callback(
        std::function<void(ApplicationInstanceSetStatusData)> callback);

    // Sends a application instance get status request message.
    // Todo: purpose/when should this be called.
    bool send_application_instance_get_status();

    // Sends a application instance set status request message.
    // Todo: purpose/when should this be called.
    bool send_application_instance_set_status(int status);

private:
    // Sends a player joined event response message to the agent when new
    // players have joined.
    bool send_player_joined_event(int num_players);
    // Sends a player left response message to the agent when players have left.
    bool send_player_left(int num_players);
    // Sends a application instance information request message to the agent.
    bool send_application_instance_information_request();
    // Sends a host information request message to the agent.
    bool send_host_information_request();

    // Callbacks potentially called by the arcus server.
    static void soft_stop(void *userdata, int timeout_seconds);
    static void allocated(void *userdata, void *allocated);
    static void meta_data(void *userdata, void *meta_data);
    static void host_information(void *userdata, void *information);
    static void application_instance_information(void *userdata, void *information);
    static void application_instance_get_status(void *userdata, int status);
    static void application_instance_set_status(void *userdata, int code);

    // Callback that sends out live_state response message by the server.
    static void live_state_request(void *userdata);

    // ancillary function to show how to parse the message payloads.
    static bool extract_allocated_payload(OneArrayPtr array,
                                          AllocatedData &allocated_data);
    static bool extract_meta_data_payload(OneArrayPtr array, MetaDataData &meta_data);
    static bool extract_host_information_payload(OneObjectPtr object,
                                                 HostInformationData &information);
    static bool extract_application_instance_information_payload(
        OneObjectPtr object, ApplicationInstanceInformationData &information);

    // The Arcus Server itself.
    OneServerPtr _server;
    const unsigned int _port;

    // Cached messages used to send the different predefined Arcus Messages
    // types.
    OneMessagePtr _live_state;
    OneMessagePtr _player_joined;
    OneMessagePtr _player_left;
    OneMessagePtr _host_information;
    OneMessagePtr _application_instance_information;
    OneMessagePtr _application_instance_get_status;
    OneMessagePtr _application_instance_set_status;

    bool host_information_request_sent;
    bool application_instance_information_request_sent;

    GameState _game_state;
    GameState _last_update_game_state;

    // Callbacks that can be set by game to be notified of events received from
    // the Arcus Server.
    std::function<void(int, void *)> _soft_stop_callback;
    void *_soft_stop_userdata;

    std::function<void(const AllocatedData &, void *)> _allocated_callback;
    void *_allocated_userdata;

    std::function<void(const MetaDataData &, void *)> _meta_data_callback;
    void *_meta_data_userdata;

    std::function<void(HostInformationData)> _host_information_callback;
    std::function<void(ApplicationInstanceInformationData)>
        _application_instance_information_callback;
    std::function<void(ApplicationInstanceGetStatusData)>
        _application_instance_get_status_callback;
    std::function<void(ApplicationInstanceSetStatusData)>
        _application_instance_set_status_callback;
};

}  // namespace game
