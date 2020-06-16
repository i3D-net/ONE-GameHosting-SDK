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
/// OneMessageApi is used to work with messages either received from or sent to
/// the OneHostApi.
///
struct OneMessageApi {
    ///
    /// Prepares a new outgoing message. Must be called to start setting
    /// values on the message. end_outgoing must be called when finished.
    ///
    OneMessagePtr (*create)(int* error);

    /// Must be called whenever finished with a message.
    void (*free)(OneMessagePtr message, int* error);

    int (*set_code)(OneMessagePtr message);
    int (*code)(OneMessagePtr message);

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

    //--------------------------------------------------------------------------
    // Message Arrays.

    // ...

    //--------------------------------------------------------------------------
    // Message Objects.
};
typedef OneMessageApi* OneMessageApiPtr;

///
/// The One Game Host API provides access to the One Game Host interfaces.
/// The One Game Host communicates with the One Platform. It
///     - accepts a connection from One to send and receive messages
///     - provides an interface for the game server to interact with those
///       messages
/// 
struct OneHostApi {
    //--------------------------------------------------------------------------
    // Game Host Life Cycle.

    OneGameHostPtr (*create)(void);
    void (*destroy)(OneGameHostPtr);

    ///
    /// Update the host. This must be called frequently in to process incoming
    /// communications and push them onto the message stack for reading.
    ///
    void (*update)(OneGameHostPtr host, int* error);

    ///
    /// Returns the status of the host's listen connection.
    /// \sa listen.
    ///
    void (*status)(OneGameHostPtr host, int* error);

    ///
    /// Listens for messages on the given port.
    ///
    void (*listen)(OneGameHostPtr host, unsigned int port, int* error);

    ///
    /// Stops listening for messages.
    ///
    void (*close)(OneGameHostPtr host, int* error);

    //--------------------------------------------------------------------------
    // Outgoing messages.

    // Todo: somewhere we would define the outgoing opcodes and format, e.g.
    // for the Metadata opcode 0x40, we would define that opcode somewhere and
    // instructions on its use.

    void (*send)(OneGameHostPtr host, OneMessagePtr message, int* error);

    //--------------------------------------------------------------------------
    // Incoming Message callbacks.

    // Todo:
    // - the customer uses these to be notified of incoming messages directed
    // at the game server
    // - the callbacks are called during processing of the update call on the
    // host
    // - if the callbacks are not set, then the messages are ignored
    // - extract to separate api struct.
    void (*set_soft_stop_callback)(OneGameHostPtr host, void(*cb)(int timeout));
    void (*set_allocated_callback)(OneGameHostPtr host, void(*cb)(void));
    void (*set_server_info_request_callback)(OneGameHostPtr host, void(*cb)(void));
    // - customer must call free_message when finished
    void (*set_custom_command_callback)(OneGameHostPtr host, void(*cb)(OneMessagePtr message));
};
typedef OneHostApi* OneHostApiPtr;

///
/// The One Game Hosting API provides access to all One interfaces.
/// 
struct OneGameHostingApi {
    OneHostApiPtr host_api; // Main API.
    OneMessageApiPtr message_api; // For working with messages.
};
typedef OneGameHostingApi* OneGameHostingApiPtr;

//------------------------------------------------------------------------------
// API Access.

// Main entry point call to get a reference to the API.
ONE_EXPORT OneGameHostingApiPtr ONE_STDCALL one_game_hosting_api();

#ifdef __cplusplus
};
#endif
