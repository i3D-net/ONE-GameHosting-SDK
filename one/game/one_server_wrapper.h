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

namespace game {

class OneServerWrapper final {
public:
    OneServerWrapper(unsigned int port);
    OneServerWrapper(const OneServerWrapper &) = delete;
    OneServerWrapper &operator=(const OneServerWrapper &) = delete;
    ~OneServerWrapper();

    enum class Status {
        uninitialized = 0,
        initialized,
        waiting_for_client,
        handshake,
        ready,
        error,
        unknown
    };

    bool init();
    void shutdown();
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

private:
    // Sends error response to the agent.
    // Todo: figure out the proper expected error_response usage.
    void send_error_response();
    // Sends a host information request message to the agent.
    bool send_host_information_request();

    // Callbacks potentially called by the arcus server.
    static void soft_stop(void *userdata, int timeout_seconds);
    static void allocated(void *userdata, void *allocated);
    static void meta_data(void *userdata, void *meta_data);

    // Callback that sends out live_state response message by the server.
    static void live_state_request(void *userdata);

    // ancillary function to show how to parse the message payloads.
    static bool extract_allocated_payload(OneArrayPtr array,
                                          AllocatedData &allocated_data);
    static bool extract_meta_data_payload(OneArrayPtr array, MetaDataData &meta_data);

    OneServerPtr _server;
    OneMessagePtr _error;
    OneMessagePtr _live_state;
    OneMessagePtr _host_information;
    OneArrayPtr _allocated;
    OneArrayPtr _meta_data;
    const unsigned int _port;

    bool host_information_request_sent;
    GameState _game_state;

    // Callbacks that can be set by game to be notified of events received from
    // the Arcus Server.
    std::function<void(int)> _soft_stop_callback;
    std::function<void(AllocatedData)> _allocated_callback;
    std::function<void(MetaDataData)> _meta_data_callback;
};

}  // namespace game
