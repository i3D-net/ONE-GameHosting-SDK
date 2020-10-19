#pragma once

#include <functional>
#include <mutex>
#include <string>

//----------------------------------------------
// One SDK object types forward declarations

struct OneServer;
typedef OneServer *OneServerPtr;

struct OneArray;
typedef OneArray *OneArrayPtr;

struct OneObject;
typedef OneObject *OneObjectPtr;

namespace one_integration {

/// OneServerWrapper encapsulates the integration for the One Arcus Server and
/// provides a game interface that hides the One Arcus Server API implementation
/// from the game. Errors are handled directly in the implementation of the
/// wrapper.
///
/// This is provided as a potential copy-paste initial integration solution for
/// users that would like to hide the c-api under a game-specific C++ class, and
/// also as a complete example of an integration.
class OneServerWrapper final {
public:
    OneServerWrapper();
    OneServerWrapper(const OneServerWrapper &) = delete;
    OneServerWrapper &operator=(const OneServerWrapper &) = delete;
    ~OneServerWrapper();

    //------------
    // Life cycle.

    // alloc and free are optional allocation override handlers. Both may be nullptr,
    // otherwise both are required.
    bool init(std::function<void *(size_t)> alloc, std::function<void(void *)> free,
              std::function<void *(void *, size_t)> realloc);
    bool listen(unsigned int port);  // Must be successful before calling update.
    void shutdown();

    // Must called often (e.g. each frame). Updates the Arcus Server, which
    // processes incoming and outgoing messages.
    void update(bool quiet);

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

    //---------------
    // Arcus setters.

    struct GameState {
        GameState() : players(0), max_players(0), name(), map(), mode(), version() {}

        int players;          // Game number of players.
        int max_players;      // Game max number of players.
        std::string name;     // Server name.
        std::string map;      // Game map.
        std::string mode;     // Game mode.
        std::string version;  // Game version.

        // Add extra custom game-specific properties here, if needed. Then
        // these need to be added to a OneObjectPtr in the implementation
        // and passed to the c_api. See the cpp file for an example.
    };
    // Set the game state to the current value. The wrapper uses this to send
    // the current state to the One Platform, when requested to do so.
    void set_game_state(const GameState &);

    // As defined in:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#applicationinstance-set-status-request
    enum class ApplicationInstanceStatus { starting = 3, online = 4, allocated = 5 };

    // Sets the Arcus application instance status. The game server must set
    // the status to starting during initialization, online once the server is
    // ready for matchmaking. Allocated is optional when directed for allocation
    // by Arcus, depending on the matchmaking design and features used.
    void set_application_instance_status(ApplicationInstanceStatus status);

    //------------------------
    // Incoming Arcus Messages

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
    // Allows the game server to be notified of an incoming Allocated message.
    void set_allocated_callback(
        std::function<void(const AllocatedData &data, void *userdata)> callback,
        void *userdata);

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
    // Allows the game server to be notified of an incoming Metadata message.
    void set_metadata_callback(
        std::function<void(const MetaDataData &data, void *userdata)> callback,
        void *userdata);

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
    // Allows the game server to be notified of an incoming Host Information message.
    void set_host_information_callback(
        std::function<void(const HostInformationData &data, void *userdata)> callback,
        void *userdata);

    // The application instance information response has a payload as defined at:
    // https://www.i3d.net/docs/one/odp/Game-Integration/Management-Protocol/Arcus-V2/request-response/#applicationinstance-information-response
    // In this example only a handfull of fields are used for simplicity.
    struct ApplicationInstanceInformationData {
        ApplicationInstanceInformationData() : fleet_id(), host_id(0), is_virtual(0) {}

        std::string fleet_id;  // fleet id.
        int host_id;           // host id.
        int is_virtual;        // is virtual.
        // ... add members as needed.
    };
    // Allows the game server to be notified of an incoming Application
    // Instance Information message.
    void set_application_instance_information_callback(
        std::function<void(const ApplicationInstanceInformationData &data,
                           void *userdata)>
            callback,
        void *userdata);

private:
    // Callbacks potentially called by the arcus server.
    static void soft_stop(void *userdata, int timeout_seconds);
    static void allocated(void *userdata, void *allocated);
    static void metadata(void *userdata, void *metadata);
    static void host_information(void *userdata, void *information);
    static void application_instance_information(void *userdata, void *information);

    // Parsing of message payloads.
    static bool extract_allocated_payload(OneArrayPtr array,
                                          AllocatedData &allocated_data);
    static bool extract_metadata_payload(OneArrayPtr array, MetaDataData &metadata);
    static bool extract_host_information_payload(OneObjectPtr object,
                                                 HostInformationData &information);
    static bool extract_application_instance_information_payload(
        OneObjectPtr object, ApplicationInstanceInformationData &information);

    // The Arcus Server itself.
    mutable std::mutex _wrapper;
    OneServerPtr _server;

    //--------------------------------------------------------------------------
    // Callbacks.

    std::function<void(int, void *)> _soft_stop_callback;
    void *_soft_stop_userdata;

    std::function<void(const AllocatedData &, void *)> _allocated_callback;
    void *_allocated_userdata;

    std::function<void(const MetaDataData &, void *)> _metadata_callback;
    void *_metadata_userdata;

    std::function<void(const HostInformationData &, void *)> _host_information_callback;
    void *_host_information_userdata;

    std::function<void(const ApplicationInstanceInformationData &, void *)>
        _application_instance_information_callback;
    void *_application_instance_information_userdata;
};

}  // namespace one_integration
