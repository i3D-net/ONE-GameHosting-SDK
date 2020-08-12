#pragma once

//------------------------------------------------------------------------------
// This is the externally facing interface for the One Game Hosting SDK.
//
// It is a C API for ABI compatibility and widespread language binding support.
//
// Users call the one_game_hosting_api function to obtain access and make calls
// against its more granular APIs.

#define ONE_EXPORT
#define ONE_STDCALL

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Errors.

typedef int OneError;

// See the C++ error.h file for error values.

//------------------------------------------------------------------------------
// Opaque types returned by Apis.

// The type operated on by the OneMessageApi.
struct OneMessage;
typedef OneMessage *OneMessagePtr;

// Type type operated on by the OneArrayApi.
struct OneArray;
typedef OneArray *OneArrayPtr;

// Type type operated on by the OneObjectApi.
struct OneObject;
typedef OneObject *OneObjectPtr;

// The type operated on by the OneServerApi, and central object to manage a One
// Server.
struct OneServer;
typedef OneServer *OneServerPtr;

//------------------------------------------------------------------------------
// Apis.

///
/// OneMessageApi is used to work with messages either received from or sent to
/// the OneServerApi.
///
struct OneMessageApi {
    ///
    /// Prepares a new outgoing message. Must be called to start setting
    /// values on the message. end_outgoing must be called when finished.
    ///
    int (*create)(OneMessagePtr *message);

    /// Must be called whenever finished with a message.
    void (*destroy)(OneMessagePtr *message);

    int (*init)(OneMessagePtr message, int code, const char *data, unsigned int size);
    int (*code)(OneMessagePtr message, int *code);

    // Getters.
    int (*val_int)(OneMessagePtr message, const char *key, int *val);
    int (*val_string)(OneMessagePtr message, const char *key, char **val);
    int (*val_array)(OneMessagePtr message, const char *key, OneArrayPtr val);
    int (*val_object)(OneMessagePtr message, const char *key, OneObjectPtr val);

    // Setters.
    int (*set_val_int)(OneMessagePtr message, const char *key, int val);
    int (*set_val_string)(OneMessagePtr message, const char *key, const char *val);
    int (*set_val_array)(OneMessagePtr message, const char *key, OneArrayPtr val);
    int (*set_val_object)(OneMessagePtr message, const char *key, OneObjectPtr val);
};
typedef OneMessageApi *OneMessageApiPtr;

///
/// OneMessagePrepareApi is used to streamlie working with messages. Providing prepare helpers &
/// others.
///
struct OneMessagePrepareApi {
    /// Must be called whenever finished with a message.
    int (*prepare_live_state)(OneMessagePtr message, int player, int max_player, const char *name,
                              const char *map, const char *mode, const char *version);
};
typedef OneMessagePrepareApi *OneMessagePrepareApiPtr;

///
/// OneArrayApi is used to work with an array to retrieve from or set in the
/// OneMessageApi.
///
struct OneArrayApi {
    // ...
};
typedef OneArrayApi *OneArrayApiPtr;

///
/// OneObjectApi is used to work with an object to retrieve from or set in the
/// OneMessageApi.
///
struct OneObjectApi {
    // ...
};
typedef OneObjectApi *OneObjectApiPtr;

///
/// The Server API is the main api used to work with the Server.
/// It:
///     - allows creating and destroying a server
///     - accepts a connection from One to send and receive messages
///     - provides an interface for the game to interact with those
///       messages
///     - is thread-safe
///
struct OneServerApi {
    //--------------------------------------------------------------------------
    // One Server Life Cycle.

    int (*create)(OneServerPtr *server, size_t max_message_in, size_t max_message_out);
    void (*destroy)(OneServerPtr *server);

    ///
    /// Update the server. This must be called frequently to process incoming
    /// and outgoing communications. Incoming messages will be forwarded to their
    /// respective incoming callbacks. If the a callback for a message is not set
    /// then the message is ignored.
    ///
    int (*update)(OneServerPtr server);

    ///
    /// Returns the status of the listen connection.
    /// \sa listen.
    ///
    int (*status)(OneServerPtr const server);

    ///
    /// Listens for messages on the given port.
    ///
    int (*listen)(OneServerPtr server, unsigned int port, int queueLength);

    ///
    /// Stops listening for messages.
    ///
    int (*shutdown)(OneServerPtr server);

    //--------------------------------------------------------------------------
    // Outgoing messages.

    // Note: these will change in final V1.0 to match new incoming Arcus API
    // messages.

    // See the [One Arcus protocol documentation website](todo.html) for more
    // information.

    ///
    /// Send the Arcus API server live_state opcode message.
    /// Message Empty Content:
    /// {}
    int (*send_error_response)(OneServerPtr server, OneMessagePtr message);

    ///
    /// Send the Arcus API server live_state opcode message.
    /// Message Mandatory Content:
    /// {
    ///   players : 0,
    ///   max players : 0,
    ///   server name : "",
    ///   server map : "",
    ///   server mode : "",
    ///   server version : "",
    /// }
    int (*send_live_state_response)(OneServerPtr server, OneMessagePtr message);

    ///
    /// Send the Arcus API server live_state opcode message.
    /// Message Empty Content:
    /// {}
    int (*send_host_information_request)(OneServerPtr server, OneMessagePtr message);

    //--------------------------------------------------------------------------
    // Incoming Message callbacks.

    // Note: these will change in final V1.0 to match new incoming Arcus API
    // messages.

    // See the [One Arcus protocol documentation website](todo.html) for more
    // information.

    // Todo:
    // - register the following callbacks to notified of incoming messages directed
    // at the game server
    // - the callbacks are called during processing of the update call on the
    // server
    // - if the callbacks are not set, then the messages are ignored
    // - extract to separate api struct

    // Required: Register the callback to be notified of a soft_stop_request. The process
    // should stop at its earliest convenience. If the server process is still
    // active after the given timeout (seconds), then One will terminate the
    // process directly on the deployment.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    int (*set_soft_stop_callback)(OneServerPtr server, void (*cb)(void *data, int timeout),
                                  void *data);

    // Required: Register the callback to be notified of a allocated_request.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    // The `void *array` must be of type OneArrayPtr or the callback will error out.
    int (*set_allocated_callback)(OneServerPtr server, void (*cb)(void *data, void *array),
                                  void *data);

    // Required: Register the callback to be notified of a meta_data_request.
    // The `void *data` is the user provided & will be passed as the first argument
    // of the callback when invoked.
    // The `data` can be nullptr, the callback is responsible to use the data properly.
    // The `void *array` must be of type OneArrayPtr or the callback will error out.
    int (*set_meta_data_callback)(OneServerPtr server, void (*cb)(void *data, void *array),
                                  void *data);

    // Required: Register to be notified of when the game must call
    // live_state_request.
    int (*set_live_state_request_callback)(OneServerPtr server, void (*cb)(void *data), void *data);
};
typedef OneServerApi *OneServerApiPtr;

///
/// Allows for control of all allocations made within the SDK.
///
struct OneAllocatorApi {
    /// Provide custom memory alloc.
    /// Must be set at init time, before using any other APIs.
    void (*set_alloc)(void *(cb)(unsigned int size));

    /// Provide custom memory free.
    /// Must be set at init time, before using any other APIs.
    void (*set_free)(void(cb)(void *));
};
typedef OneAllocatorApi *OneAllocatorApiPtr;

///
/// The One Game Hosting API provides access to all One interfaces. Call
/// ::one_game_hosting_api to obtain access to an instance of this struct.
///
struct OneGameHostingApi {
    OneServerApiPtr
        server_api;  /// Main API to create a Game Hosting Server that communicates with One.
    OneMessageApiPtr message_api;                 /// For working with messages.
    OneMessagePrepareApiPtr message_prepare_api;  /// For working with messages.
    OneArrayApiPtr array_api;       /// For working with array types contained in messages.
    OneObjectApiPtr object_api;     /// For working with object types contained in messages.
    OneAllocatorApi allocator_api;  /// For providing custom allocation.
};
typedef OneGameHostingApi *OneGameHostingApiPtr;

//------------------------------------------------------------------------------
// API Access.

///
/// Main entry point call to get a reference to the APIs.
/// Sub-Apis can be referenced like so:
/// OneGameHOstingApiPtr one = one_game_hosting_api();
/// OneServerApiPtr server_api = one->server_api();
/// server_api->do_something(); // Fictional example API function.
///
ONE_EXPORT OneGameHostingApiPtr ONE_STDCALL one_game_hosting_api(void);

#ifdef __cplusplus
};
#endif
