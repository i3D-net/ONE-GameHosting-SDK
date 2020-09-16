#pragma once

#include <functional>
#include <string>

// Forward declarations for One SDK object types.
struct OneServer;
typedef OneServer *OneServerPtr;

struct OneMessage;
typedef OneMessage *OneMessagePtr;

struct OneArray;
typedef OneArray *OneArrayPtr;

struct OneObject;
typedef OneObject *OneObjectPtr;

namespace game {

class TestOneServerWrapper;

class OneServerWrapper final {
public:
    OneServerWrapper(unsigned int port);
    OneServerWrapper(const OneServerWrapper &) = delete;
    OneServerWrapper &operator=(const OneServerWrapper &) = delete;
    ~OneServerWrapper();

    bool init();
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
        int players;          // Game number of players.
        int max_players;      // Game max number of players.
        std::string name;     // Server name.
        std::string map;      // Game map.
        std::string mode;     // Game mode.
        std::string version;  // Game version.
    };
    void set_game_state(const GameState &);
    void update();

    void set_soft_stop_callback(std::function<void(int)> callback);

    // The allocation request has a optional body containing a JSON object with key value
    // pairs for meta data this keys and values are definable by the customer.
    // The current values are matching the payload shown in the documentation at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#allocated-request
    struct AllocatedData {
        std::string max_players;  // Game max number of players.
        std::string map;          // Game map.
    };
    void set_allocated_callback(std::function<void(AllocatedData)> callback);

    // The meta data request has a optional body containing a JSON object with key value
    // pairs for meta data this keys and values are definable by the customer.
    // The current values are matching the payload shown in the documentation at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#meta-data-request
    struct MetaDataData {
        std::string map;   // Game map.
        std::string mode;  // Game mode.
        std::string type;  // Game type.
    };
    void set_meta_data_callback(std::function<void(MetaDataData)> callback);

    // The host information response has a payload as defined at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#host-information-response
    // In this example only a handfull of fields are used for simplicity.
    struct HostInformationData {
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
        int status;  // status.
    };
    void set_application_instance_get_status_callback(
        std::function<void(ApplicationInstanceGetStatusData)> callback);

    // The application instancate information response has a payload as defined at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#applicationinstance-set-status-response
    // In this example only a handfull of fields are used for simplicity.
    struct ApplicationInstanceSetStatusData {
        int code;  // code.
    };
    void set_application_instance_set_status_callback(
        std::function<void(ApplicationInstanceSetStatusData)> callback);

    // Sends a application instance get status request message.
    bool send_application_instance_get_status();

    // Sends a application instance set status request message.
    bool send_application_instance_set_status(int status);

private:
    // Sends a player joined event response message to the agent when new players have
    // joined.
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

    OneServerPtr _server;
    OneMessagePtr _live_state;
    OneMessagePtr _player_joined;
    OneMessagePtr _player_left;
    OneMessagePtr _host_information;
    OneMessagePtr _application_instance_information;
    OneMessagePtr _application_instance_get_status;
    OneMessagePtr _application_instance_set_status;

    const unsigned int _port;

    bool host_information_request_sent;
    bool application_instance_information_request_sent;
    GameState _game_state;
    GameState _last_update_game_state;

    // Callbacks that can be set by game to be notified of events received from
    // the Arcus Server.
    std::function<void(int)> _soft_stop_callback;
    std::function<void(AllocatedData)> _allocated_callback;
    std::function<void(MetaDataData)> _meta_data_callback;
    std::function<void(HostInformationData)> _host_information_callback;
    std::function<void(ApplicationInstanceInformationData)>
        _application_instance_information_callback;
    std::function<void(ApplicationInstanceGetStatusData)>
        _application_instance_get_status_callback;
    std::function<void(ApplicationInstanceSetStatusData)>
        _application_instance_set_status_callback;

    friend TestOneServerWrapper;
};

class TestOneServerWrapper final {
public:
    static bool extract_allocated_payload(
        OneArrayPtr array, OneServerWrapper::AllocatedData &allocated_data);
    static bool extract_meta_data_payload(OneArrayPtr array,
                                          OneServerWrapper::MetaDataData &meta_data);

private:
    TestOneServerWrapper() = delete;
    ~TestOneServerWrapper() = delete;
};

}  // namespace game
