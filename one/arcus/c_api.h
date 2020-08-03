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

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Errors.

// This section will contain integer error codes returned throughout the
// api as well as a method to convert the human-readable strings for easier
// logging and debugging.

// A typedef will be provided instead of the ints currently present below.

//------------------------------------------------------------------------------
// Opaque types returned by Apis.

// The type operated on by the OneMessageApi.
struct OneMessage;
typedef OneMessage* OneMessagePtr;

// Type type operated on by the OneArrayApi.
struct OneArray;
typedef OneArray* OneArrayPtr;

// Type type operated on by the OneObjectApi.
struct OneObject;
typedef OneObject* OneObjectPtr;

// The type operated on by the OneServerApi, and central object to manage a One
// Server.
struct OneServer;
typedef OneServer* OneServerPtr;

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
    OneMessagePtr (*create)(int* error);

    /// Must be called whenever finished with a message.
    void (*free)(OneMessagePtr message, int* error);

    void (*set_code)(OneMessagePtr message, int code, int* error);
    int (*code)(OneMessagePtr message, int* error);

    // Getters.
    int  (*val_int)(OneMessagePtr message, const char * key, int* error);
    const char * (*val_string)(OneMessagePtr message, const char * key, int* error);
    OneArrayPtr  (*val_array)(OneMessagePtr message, const char * key, int* error);
    OneObjectPtr  (*val_object)(OneMessagePtr message, const char * key, int* error);

    // Setters.
    void (*set_val_int)(OneMessagePtr message, const char * key, int val, int* error);
    void (*set_val_string)(OneMessagePtr message, const char * key, const char* val, int* error);
    void (*set_val_array)(OneMessagePtr message, const char * key, OneArrayPtr val, int* error);
    void (*set_val_object)(OneMessagePtr message, const char * key, OneObjectPtr val, int* error);
};
typedef OneMessageApi* OneMessageApiPtr;

///
/// OneArrayApi is used to work with an array to retrieve from or set in the
/// OneMessageApi.
///
struct OneArrayApi {
    // ...
};
typedef OneArrayApi* OneArrayApiPtr;

///
/// OneObjectApi is used to work with an object to retrieve from or set in the
/// OneMessageApi.
///
struct OneObjectApi {
    // ...
};
typedef OneObjectApi* OneObjectApiPtr;

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

    OneServerPtr (*create)(void);
    void (*destroy)(OneServerPtr);

    ///
    /// Update the server. This must be called frequently to process incoming
    /// and outgoing communications. Incoming messages will be forwarded to their
    /// respective incoming callbacks. If the a callback for a message is not set
    /// then the message is ignored.
    ///
    void (*update)(OneServerPtr server, int* error);

    ///
    /// Returns the status of the listen connection.
    /// \sa listen.
    ///
    void (*status)(OneServerPtr server, int* error);

    ///
    /// Listens for messages on the given port.
    ///
    void (*listen)(OneServerPtr server, unsigned int port, int* error);

    ///
    /// Stops listening for messages.
    ///
    void (*close)(OneServerPtr server, int* error);

    //--------------------------------------------------------------------------
    // Outgoing messages.

    // Note: these will change in final V1.0 to match new incoming Arcus API
    // messages.

    // See the [One Arcus protocol documentation website](todo.html) for more
    // information.

    ///
    /// Send the Arcus API server metadata opcode message.
    ///
    void (*send_metadata)(OneServerPtr server, OneArrayPtr data, int* error);

    // Todo: struct containing required server info config fields that must be
    // sent...to be passed here by caller.
    void (*send_server_info)(OneServerPtr server, OneArrayPtr data, int* error);

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

    // Required: Register the callback to be notified of a soft stop. The process
    // should stop at its earliest convenience. If the server process is still
    // active after the given timeout (seconds), then One will terminate the
    // process directly on the deployment.
    void (*set_soft_stop_callback)(OneServerPtr server, void(*cb)(int timeout));

    // Required: Register the callback to be notified of when the server has been
    // allocated for matchmaking.
    void (*set_allocated_callback)(OneServerPtr server, void(*cb)(void));

    // Required: Register to be notified of when the game must call
    // send_server_info.
    void (*set_server_info_request_callback)(OneServerPtr server, void(*cb)(void));
};
typedef OneServerApi* OneServerApiPtr;

///
/// Allows for control of all allocations made within the SDK.
///
struct OneAllocatorApi {
    /// Provide custom memory alloc.
    /// Must be set at init time, before using any other APIs.
    void (*set_alloc)(void*(cb)(unsigned int size));

    /// Provide custom memory free.
    /// Must be set at init time, before using any other APIs.
    void (*set_free)(void (cb)(void*));
};
typedef OneAllocatorApi* OneAllocatorApiPtr;

///
/// The One Game Hosting API provides access to all One interfaces. Call
/// ::one_game_hosting_api to obtain access to an instance of this struct.
/// 
struct OneGameHostingApi {
    OneServerApiPtr server_api; /// Main API to create a Game Hosting Server that communicates with One.
    OneMessageApiPtr message_api; /// For working with messages.
    OneArrayApiPtr array_api; /// For working with array types contained in messages.
    OneObjectApiPtr object_api; /// For working with object types contained in messages.
    OneAllocatorApi allocator_api; /// For providing custom allocation.
};
typedef OneGameHostingApi* OneGameHostingApiPtr;

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