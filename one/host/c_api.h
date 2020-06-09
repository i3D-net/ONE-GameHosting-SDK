#pragma once

//------------------------------------------------------------------------------
// This is the externally facing interface for the One Game Host SDK.
//
// It is a C API for ABI compatibility and widespread language binding support.
//
// Users call the one_game_host_api function to obtain access and make calls
// against its members.

#define ONE_EXPORT
#define ONE_STDCALL

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Errors.

//------------------------------------------------------------------------------
// One Game Host.

struct OneGameHost;
typedef OneGameHost* OneGameHostPtr;

struct OneOutMessage;
typedef OneOutMessage* OneMessagePtr;

struct OneArray;
typedef OneArray* OneArrayPtr;

struct OneObject;
typedef OneObject* OneObjectPtr;

///
/// The One Game Host API provides access to the One Game Host interfaces.
/// The One Game Host communicates with the One Platform. It
///     - accepts a connection from One to send and receive messages
///     - provides an interface for the game server to interact with those
///       messages
/// 
struct OneGameHostApi {
    //--------------------------------------------------------------------------
    // Game Host Life Cycle.

    OneGameHostPtr (*create_host)(void);
    void (*destroy_host)(OneGameHostPtr);

    ///
    /// Update the host. This must be called frequently in to process incoming
    /// communications and push them onto the message stack for reading.
    ///
    void (*update)(OneGameHostPtr, int* error);

    ///
    /// Returns the status of the host's listen connection.
    /// \sa listen.
    ///
    void (*status)(int* error);

    ///
    /// Listens for messages on the given port.
    ///
    void (*listen)(unsigned int port, int* error);

    ///
    /// Stops listening for messages.
    ///
    void (*close)(int* error);

    //--------------------------------------------------------------------------
    // Game Host Messages - shared.

    /// Must be called whenever finished with an incoming or outgoing message.
    void (*free_message)(OneMessagePtr message, int* error);

    int (*set_message_code)(OneMessagePtr message);
    int (*message_code)(OneMessagePtr message);

    //--------------------------------------------------------------------------
    // Game Host Outgoing Messages.

    ///
    /// Prepares a new outgoing message. Must be called to start setting
    /// values on the message. end_outgoing must be called when finished.
    ///
    OneMessagePtr (*create_outgoing)(int* error);

    void (*set_message_int)(OneMessagePtr message, const char * key, int val, int* error);
    void (*set_message_string)(OneMessagePtr message, const char * key, const char* val, int* error);
    void (*set_message_array)(OneMessagePtr message, const char * key, OneArrayPtr val, int* error);
    void (*set_message_object)(OneMessagePtr message, const char * key, OneObjectPtr val, int* error);

    void (*finalize_outgoing)(OneMessagePtr message, int* error);

    //--------------------------------------------------------------------------
    // Game Host Incoming Messages.

    ///
    /// \param error Returns [TBD error not found] if there are no incoming
    ///              messages remaining. `free_message` should not need be
    ///              called if there is no incoming message.
    ///
    OneMessagePtr (*pop_incoming)(int* error);

    int  (*get_int)(OneMessagePtr message, const char * key, int* error);
    const char * (*message_string)(OneMessagePtr message, const char * key, int* error);
    OneArrayPtr  (*message_array)(OneMessagePtr message, const char * key, int* error);
    OneObjectPtr  (*message_object)(OneMessagePtr message, const char * key, int* error);

    //--------------------------------------------------------------------------
    // Message Arrays.

    // ...

    //--------------------------------------------------------------------------
    // Message Objects.
};
typedef OneGameHostApi* OneGameHostApiPtr;

//------------------------------------------------------------------------------
// API Access.

// Main entry point call to get a reference to the API.
ONE_EXPORT OneGameHostApiPtr ONE_STDCALL one_game_host_api();

#ifdef __cplusplus
};
#endif
