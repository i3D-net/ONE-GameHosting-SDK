#pragma once

#include <functional>
#include <string>

// Forward declarations for One SDK object types.
struct OneServer;
struct OneMessage;
struct OneMessage;
struct OneMessage;

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

    void init();
    void shutdown();
    Status status() const;

    struct GameState {
        int players;
        int max_players;
        std::string name;  // Server name.
        std::string map;
        std::string mode;     // Game mode.
        std::string version;  // Game Server version.
    };
    void set_game_state(const GameState &);
    void update();

    void set_soft_stop_callback(std::function<void(int)>);

private:
    // Callbacks potentially called by the arcus server.
    static void soft_stop(void *userdata, int timeout_seconds);

    OneServer *_server;
    OneMessage *_error;
    OneMessage *_live_state;
    OneMessage *_host_information;
    const unsigned int _port;

    GameState _game_state;

    // Callbacks that can be set by game to be notified of events received from
    // the Arcus Server.
    std::function<void(int)> _soft_stop_callback;
};

}  // namespace game
