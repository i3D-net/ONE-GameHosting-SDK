#pragma once

//------------------------------------------------------------------------------
// This is the externally facing interface for the One Game Hosting SDK.
//
// It is a C API for ABI compatibility and widespread language binding support.
//
// Users call the one_game_hosting_api function to obtain access and make calls
// against its members.

#define ONE_EXPORT
#define ONE_STDCALL

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// Errors.

//------------------------------------------------------------------------------
// One Game Server.

struct OneServer;
typedef OneServer* OneServerPtr;

struct OneMessage;
typedef OneMessage* OneMessagePtr;

struct OneArray;
typedef OneArray* OneArrayPtr;

struct OneObject;
typedef OneObject* OneObjectPtr;


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

    //--------------------------------------------------------------------------
    // Message Arrays.

    // ...

    //--------------------------------------------------------------------------
    // Message Objects.
};
typedef OneMessageApi* OneMessageApiPtr;

///
/// The Server API is the main api used to work with the Server..
/// It allows creating and destroying a server and communication with the
/// One Platform. It
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
    /// Update the server. This must be called frequently in to process incoming
    /// communications and push them onto the message stack for reading.
    ///
    void (*update)(OneServerPtr server, int* error);

    ///
    /// Returns the status of the host's listen connection.
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

    // Todo: somewhere we would define the outgoing opcodes and format, e.g.
    // for the Metadata opcode 0x40, we would define that opcode somewhere and
    // instructions on its use.

    ///
    /// Send a message to the One platform. The message must have its opcode
    /// set. If it is a standard One API opcode, then the required keys must
    /// be present and match the expected schema.
    ///
    void (*send)(OneServerPtr server, OneMessagePtr message, int* error);

    //--------------------------------------------------------------------------
    // Incoming Message callbacks.

    // Todo:
    // - the customer uses these to be notified of incoming messages directed
    // at the game server
    // - the callbacks are called during processing of the update call on the
    // server
    // - if the callbacks are not set, then the messages are ignored
    // - extract to separate api struct
    // - if the API changes, for example if the "soft stop" one message changes
    // on the one platform, then these function signatures would change and the
    // customer would need to update their integration code
    void (*set_soft_stop_callback)(OneServerPtr server, void(*cb)(int timeout));
    void (*set_allocated_callback)(OneServerPtr server, void(*cb)(void));
    void (*set_server_info_request_callback)(OneServerPtr server, void(*cb)(void));
    void (*set_custom_command_callback)(OneServerPtr server, void(*cb)(OneMessagePtr message));
};
typedef OneServerApi* OneServerApiPtr;

///
/// The One Game Hosting API provides access to all One interfaces. Call
/// one_game_hosting_api(void) to obtain access to an instance of this struct.
/// 
struct OneGameHostingApi {
    OneServerApiPtr server_api; // Main API.
    OneMessageApiPtr message_api; // For working with messages.
};
typedef OneGameHostingApi* OneGameHostingApiPtr;

//------------------------------------------------------------------------------
// API Access.

///
/// Main entry point call to get a reference to the API.
///
ONE_EXPORT OneGameHostingApiPtr ONE_STDCALL one_game_hosting_api(void);

#ifdef __cplusplus
};
#endif
