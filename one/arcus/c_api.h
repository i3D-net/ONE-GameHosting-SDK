#pragma once

#include <cstddef>

//------------------------------------------------------------------------------
// This is the externally facing interface for the One Game Hosting SDK.
//
// It is a C API for ABI compatibility and widespread language binding support.
//
// Users call the one_game_hosting_api function to obtain access and make calls
// against its more granular APIs.

#define ONE_EXPORT
#define ONE_STDCALL

#include <one/arcus/c_error.h>

#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

// See c_error.h file for error values.

// Server status values.
typedef enum OneServerStatus {
    ONE_SERVER_STATUS_UNINITIALIZED = 0,
    ONE_SERVER_STATUS_INITIALIZED,
    ONE_SERVER_STATUS_WAITING_FOR_CLIENT,
    ONE_SERVER_STATUS_HANDSHAKE,
    ONE_SERVER_STATUS_READY,
    ONE_SERVER_STATUS_ERROR
} OneServerStatus;

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

///
/// Prepares a new outgoing message. Must be called to start setting
/// values on the message. end_outgoing must be called when finished.
///
OneError one_message_create(OneMessagePtr *message);

/// Must be called whenever finished with a message.
void one_message_destroy(OneMessagePtr *message);

/// Must be called to initialize a message from a string.
OneError one_message_init(OneMessagePtr message, int code, const char *data,
                          unsigned int size);

/// Must be called to reset a message.
OneError one_message_reset(OneMessagePtr message);

/// Getters.
OneError one_message_code(OneMessagePtr message, int *code);

OneError one_message_is_val_bool(OneMessagePtr message, const char *key, bool *result);
OneError one_message_is_val_int(OneMessagePtr message, const char *key, bool *result);
OneError one_message_is_val_string(OneMessagePtr message, const char *key, bool *result);
OneError one_message_is_val_array(OneMessagePtr message, const char *key, bool *result);
OneError one_message_is_val_object(OneMessagePtr message, const char *key, bool *result);

OneError one_message_val_bool(OneMessagePtr message, const char *key, bool *val);
OneError one_message_val_int(OneMessagePtr message, const char *key, int *val);

// The size is the number of characters in the string, excluding any trailing '\0'.
OneError one_message_val_string_size(OneMessagePtr message, const char *key,
                                     size_t *size);
// The content of the value is copied into val & val_size must be the size available in
// val. There are no '\0' appended at the end.
OneError one_message_val_string(OneMessagePtr message, const char *key, char *val,
                                size_t val_size);
OneError one_message_val_array(OneMessagePtr message, const char *key, OneArrayPtr val);
OneError one_message_val_object(OneMessagePtr message, const char *key, OneObjectPtr val);

/// Setters.
OneError one_message_set_val_bool(OneMessagePtr message, const char *key, bool val);
OneError one_message_set_val_int(OneMessagePtr message, const char *key, int val);
OneError one_message_set_val_string(OneMessagePtr message, const char *key,
                                    const char *val);
OneError one_message_set_val_array(OneMessagePtr message, const char *key,
                                   OneArrayPtr val);
OneError one_message_set_val_object(OneMessagePtr message, const char *key,
                                    OneObjectPtr val);

///
/// OneArrayApi is used to work with arrays use in the messages.
///

OneError one_array_create(OneArrayPtr *array);

/// Must be called whenever finished with an array.
void one_array_destroy(OneArrayPtr *array);

OneError one_array_copy(OneArrayPtr source, OneArrayPtr destination);
OneError one_array_clear(OneArrayPtr array);
OneError one_array_reserve(OneArrayPtr array, size_t size);
OneError one_array_is_empty(OneArrayPtr array, bool *empty);
OneError one_array_size(OneArrayPtr array, size_t *size);
OneError one_array_capacity(OneArrayPtr array, size_t *capacity);

OneError one_array_push_back_bool(OneArrayPtr array, bool val);
OneError one_array_push_back_int(OneArrayPtr array, int val);
OneError one_array_push_back_string(OneArrayPtr array, const char *val);
OneError one_array_push_back_array(OneArrayPtr array, OneArrayPtr val);
OneError one_array_push_back_object(OneArrayPtr array, OneObjectPtr val);

OneError one_array_pop_back(OneArrayPtr array);

OneError one_array_is_val_bool(OneArrayPtr array, unsigned int pos, bool *result);
OneError one_array_is_val_int(OneArrayPtr array, unsigned int pos, bool *result);
OneError one_array_is_val_string(OneArrayPtr array, unsigned int pos, bool *result);
OneError one_array_is_val_array(OneArrayPtr array, unsigned int pos, bool *result);
OneError one_array_is_val_object(OneArrayPtr array, unsigned int pos, bool *result);

OneError one_array_val_bool(OneArrayPtr array, unsigned int pos, bool *val);
OneError one_array_val_int(OneArrayPtr array, unsigned int pos, int *val);

// The size is the number of characters in the string, excluding any trailing '\0'.
OneError one_array_val_string_size(OneArrayPtr array, unsigned int pos, size_t *size);
// The content of the value is copied into val & val_size must be the size available in
// val. There are no '\0' appended at the end.
OneError one_array_val_string(OneArrayPtr array, unsigned int pos, char *val,
                              size_t val_size);
OneError one_array_val_array(OneArrayPtr array, unsigned int pos, OneArrayPtr val);
OneError one_array_val_object(OneArrayPtr array, unsigned int pos, OneObjectPtr val);

OneError one_array_set_val_bool(OneArrayPtr array, unsigned int pos, bool val);
OneError one_array_set_val_int(OneArrayPtr array, unsigned int pos, int val);
OneError one_array_set_val_string(OneArrayPtr array, unsigned int pos, const char *val);
OneError one_array_set_val_array(OneArrayPtr array, unsigned int pos, OneArrayPtr val);
OneError one_array_set_val_object(OneArrayPtr array, unsigned int pos, OneObjectPtr val);

///
/// OneObjectApi is used to work with arrays use in the messages.
///

OneError one_object_create(OneObjectPtr *object);

/// Must be called whenever finished with an array.
void one_object_destroy(OneObjectPtr *object);

OneError one_object_copy(OneObjectPtr source, OneObjectPtr destination);
OneError one_object_clear(OneObjectPtr object);
OneError one_object_is_empty(OneObjectPtr object, bool *empty);
OneError one_object_remove_key(OneObjectPtr object, const char *key);

OneError one_object_is_val_bool(OneObjectPtr object, const char *key, bool *result);
OneError one_object_is_val_int(OneObjectPtr object, const char *key, bool *result);
OneError one_object_is_val_string(OneObjectPtr object, const char *key, bool *result);
OneError one_object_is_val_array(OneObjectPtr object, const char *key, bool *result);
OneError one_object_is_val_object(OneObjectPtr object, const char *key, bool *result);

OneError one_object_val_bool(OneObjectPtr object, const char *key, bool *val);
OneError one_object_val_int(OneObjectPtr object, const char *key, int *val);
// The size is the number of characters in the string, excluding any trailing '\0'.
OneError one_object_val_string_size(OneObjectPtr object, const char *key, size_t *size);
// The content of the value is copied into val & val_size must be the size available in
// val. There are no '\0' appended at the end.
OneError one_object_val_string(OneObjectPtr object, const char *key, char *val,
                               size_t size);
OneError one_object_val_array(OneObjectPtr object, const char *key, OneArrayPtr val);
OneError one_object_val_object(OneObjectPtr object, const char *key, OneObjectPtr val);

OneError one_object_set_val_bool(OneObjectPtr object, const char *key, bool val);
OneError one_object_set_val_int(OneObjectPtr object, const char *key, int val);
OneError one_object_set_val_string(OneObjectPtr object, const char *key, const char *val);
OneError one_object_set_val_array(OneObjectPtr object, const char *key, OneArrayPtr val);
OneError one_object_set_val_object(OneObjectPtr object, const char *key,
                                   OneObjectPtr val);

///
/// OneMessagePrepareApi is used to streamlie working with messages. Providing an easy
/// way to set the messages content.
///

OneError one_message_prepare_error_response(OneMessagePtr message);
OneError one_message_prepare_live_state_response(int player, int max_player,
                                                 const char *name, const char *map,
                                                 const char *mode, const char *version,
                                                 OneMessagePtr message);
OneError one_message_prepare_host_information_request(OneMessagePtr message);

///
/// OneArrayApi is used to work with an array to retrieve from or set in the
/// OneMessageApi.
///

///
/// OneObjectApi is used to work with an object to retrieve from or set in the
/// OneMessageApi.
///

///
/// The Server API is the main api used to work with the Server.
/// It:
///     - allows creating and destroying a server
///     - accepts a connection from One to send and receive messages
///     - provides an interface for the game to interact with those
///       messages
///     - is thread-safe
///

//--------------------------------------------------------------------------
// One Server Life Cycle.

OneError one_server_create(OneServerPtr *server);
void one_server_destroy(OneServerPtr *server);

///
/// Update the server. This must be called frequently to process incoming
/// and outgoing communications. Incoming messages will be forwarded to their
/// respective incoming callbacks. If the a callback for a message is not set
/// then the message is ignored.
///
OneError one_server_update(OneServerPtr server);

///
/// Returns the status of the listen connection.
/// \sa listen.
///
OneError one_server_status(OneServerPtr const server, int *status);

///
/// Listens for messages on the given port.
///
OneError one_server_listen(OneServerPtr server, unsigned int port);

///
/// Stops listening for messages.
///
OneError one_server_shutdown(OneServerPtr server);

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
OneError one_server_send_error_response(OneServerPtr server, OneMessagePtr message);

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
OneError one_server_send_live_state_response(OneServerPtr server, OneMessagePtr message);

///
/// Send the Arcus API server live_state opcode message.
/// Message Empty Content:
/// {}
OneError one_server_send_host_information_request(OneServerPtr server,
                                                  OneMessagePtr message);

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
OneError one_server_set_soft_stop_callback(OneServerPtr server,
                                           void (*callback)(void *data, int timeout),
                                           void *data);

// Required: Register the callback to be notified of a allocated_request.
// The `void *data` is the user provided & will be passed as the first argument
// of the callback when invoked.
// The `data` can be nullptr, the callback is responsible to use the data properly.
// The `void *array` must be of type OneArrayPtr or the callback will error out.
OneError one_server_set_allocated_callback(OneServerPtr server,
                                           void (*callback)(void *data, void *array),
                                           void *data);

// Required: Register the callback to be notified of a meta_data_request.
// The `void *data` is the user provided & will be passed as the first argument
// of the callback when invoked.
// The `data` can be nullptr, the callback is responsible to use the data properly.
// The `void *array` must be of type OneArrayPtr or the callback will error out.
OneError one_server_set_meta_data_callback(OneServerPtr server,
                                           void (*callback)(void *data, void *array),
                                           void *data);

// Required: Register to be notified of when the game must call
// live_state_request.
OneError one_server_set_live_state_request_callback(OneServerPtr server,
                                                    void (*callback)(void *data),
                                                    void *data);

///
/// OneAllocatorApi allows for control of all allocations made within the SDK.
///

/// Provide custom memory alloc.
/// Must be set at init time, before using any other APIs.
void one_allocator_set_alloc(void *(callback)(unsigned int size));

/// Provide custom memory free.
/// Must be set at init time, before using any other APIs.
void one_allocator_set_free(void(callback)(void *));

#ifdef __cplusplus
};
#endif
