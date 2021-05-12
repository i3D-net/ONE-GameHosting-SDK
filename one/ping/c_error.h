#pragma once

/** @file c_error.h
    @brief The C API error values and utilities.
**/

#include <one/ping/c_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

// Note - when adding errors:
//    1. Never change an existing number mapping.
//    2. Add the new error symbol to the error_text function in error.cpp.
typedef enum I3dPingError {
    I3D_PING_ERROR_NONE = 0,
    I3D_PING_ERROR_VALIDATION_CALLBACK_IS_NULLPTR = 100,
    I3D_PING_ERROR_VALIDATION_COUNTRY_IS_NULLPTR = 101,
    I3D_PING_ERROR_VALIDATION_DC_LOCATION_NAME_IS_NULLPTR = 102,
    I3D_PING_ERROR_VALIDATION_HOSTNAME_IS_NULLPTR = 103,
    I3D_PING_ERROR_VALIDATION_IPV4_IS_NULLPTR = 104,
    I3D_PING_ERROR_VALIDATION_IPV6_IS_NULLPTR = 105,
    I3D_PING_ERROR_VALIDATION_JSON_IS_NULLPTR = 106,
    I3D_PING_ERROR_VALIDATION_SITES_GETTER_IS_NULLPTR = 107,
    I3D_PING_ERROR_VALIDATION_PINGERS_IS_NULLPTR = 108,
    I3D_PING_ERROR_VALIDATION_STATUS_IS_NULLPTR = 109,
    I3D_PING_ERROR_VALIDATION_SIZE_IS_NULLPTR = 110,
    I3D_PING_ERROR_VALIDATION_CONTINENT_ID_IS_NULLPTR = 111,
    I3D_PING_ERROR_VALIDATION_DC_LOCATION_ID_IS_NULLPTR = 112,
    I3D_PING_ERROR_VALIDATION_IP_IS_NULLPTR = 113,
    I3D_PING_ERROR_VALIDATION_LAST_TIME_IS_NULLPTR = 114,
    I3D_PING_ERROR_VALIDATION_AVERAGE_TIME_NULLPTR = 115,
    I3D_PING_ERROR_VALIDATION_MIN_TIME_IS_NULLPTR = 116,
    I3D_PING_ERROR_VALIDATION_MAX_TIME_NULLPTR = 117,
    I3D_PING_ERROR_VALIDATION_MEDIAN_TIME_IS_NULLPTR = 118,
    I3D_PING_ERROR_VALIDATION_PING_RESPONSE_COUNT_IS_NULLPTR = 119,
    I3D_PING_ERROR_VALIDATION_RESULT_IS_NULLPTR = 120,
    I3D_PING_ERROR_VALIDATION_IP_LIST_IS_NULLPTR = 121,
    I3D_PING_ERROR_VALIDATION_HTTP_GET_CALLBACK_IS_NULLPTR = 122,
    I3D_PING_ERROR_VALIDATION_SIZE_IS_TOO_SMALL = 121,
    I3D_PING_ERROR_VALIDATION_IP_SIZE_IS_TOO_SMALL = 122,
    I3D_PING_ERROR_DATA_PARSE_FAILED = 200,
    I3D_PING_ERROR_DATA_JSON_PAYLOAD_IS_INVALID = 201,
    I3D_PING_ERROR_DATA_JSON_SERVER_INFORMATION_IS_INVALID = 202,
    I3D_PING_ERROR_SITES_GETTER_NOT_INITIALIZED = 300,
    I3D_PING_ERROR_SITES_GETTER_ALREADY_INITIALIZED = 301,
    I3D_PING_ERROR_SITES_GETTER_CALLBACK_IS_NULLPTR = 302,
    I3D_PING_ERROR_SITES_GETTER_ALLOCATION_FAILED = 303,
    I3D_PING_ERROR_SITES_GETTER_UNKWON_STATUS = 304,
    I3D_PING_ERROR_SITES_GETTER_HTTP_GET_CALLBACK_NOT_SET = 305,
    I3D_PING_ERROR_PING_SITE_POS_OUT_OF_RANGE = 306,
    I3D_PING_ERROR_PING_SITE_IP_POS_OUT_OF_RANGE = 307,
    I3D_PING_ERROR_IP_LIST_POS_IS_OUT_OF_RANGE = 400,
    I3D_PING_ERROR_IP_LIST_ALLOCATION_FAILED = 401,
    I3D_PING_ERROR_SOCKET_SYSTEM_INIT_FAIL = 500,
    I3D_PING_ERROR_SOCKET_CREATION_FAIL = 501,
    I3D_PING_ERROR_SOCKET_TTL_SET_SOCKET_OPTION_FAIL = 502,
    I3D_PING_ERROR_SOCKET_SYSTEM_CLEANUP_FAIL = 503,
    I3D_PING_ERROR_SOCKET_INVALID_IPV4 = 504,
    I3D_PING_ERROR_SOCKET_SEND_FAIL = 505,
    I3D_PING_ERROR_SOCKET_RECEIVE_BUFFER_TOO_SMALL = 506,
    I3D_PING_ERROR_SOCKET_RECEIVE_ERROR = 507,
    I3D_PING_ERROR_SOCKET_RECEIVE_TOO_FEW_BYTES = 508,
    I3D_PING_ERROR_SOCKET_RECEIVE_DESTINATION_UNREACHABLE = 509,
    I3D_PING_ERROR_SOCKET_RECEIVE_UNKNOWN_ICMP_PACKET_TYPE = 510,
    I3D_PING_ERROR_SOCKET_RECEIVE_TTL_EXPIRED = 511,
    I3D_PING_ERROR_SOCKET_INVALID_TIME = 512,
    I3D_PING_ERROR_SOCKET_ALREADY_INITIALIZED = 513,
    I3D_PING_ERROR_SOCKET_NOT_INITIALIZED = 514,
    I3D_PING_ERROR_SOCKET_PING_NOT_SENT = 515,
    I3D_PING_ERROR_SOCKET_UNKNOWN_STATUS = 516,
    I3D_PING_ERROR_SOCKET_INVALID_SEQUENCE_NUMBER = 517,
    I3D_PING_ERROR_SOCKET_SELECT_FAILED = 518,
    I3D_PING_ERROR_SOCKET_NOT_READY = 519,
    I3D_PING_ERROR_SOCKET_CANNOT_BE_RESET = 520,
    I3D_PING_ERROR_PINGER_DIVISION_BY_ZERO = 600,
    I3D_PING_ERROR_PINGER_INVALID_TIME = 601,
    I3D_PING_ERROR_PINGER_ALREADY_INITIALIZED = 602,
    I3D_PING_ERROR_PINGER_IS_UNINITIALIZED = 603,
    I3D_PING_ERROR_PINGERS_NOT_INITIALIZED = 700,
    I3D_PING_ERROR_PINGERS_ALREADY_INITIALIZED = 701,
    I3D_PING_ERROR_PINGERS_POS_IS_OUT_OF_RANGE = 702,
} I3dPingError;

I3D_PING_EXPORT bool i3d_ping_is_error(I3dPingError err);

// Given a I3dPingError, returns a string matching the symbol name, e.g.
// i3d_ping_error_text(I3D_PING_ERROR_CONNECTION_INVALID_MESSAGE_HEADER) will return
// "I3D_PING_ERROR_CONNECTION_INVALID_MESSAGE_HEADER".
I3D_PING_EXPORT const char *i3d_ping_error_text(I3dPingError err);

#ifdef __cplusplus
};
#endif
