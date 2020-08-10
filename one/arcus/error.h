#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef int Error;

// Note - when adding errors:
//    1. Never change an existing number mapping.
//    2. Add the new error symbol the string function in error.cpp.
enum {
    ONE_ERROR_NONE = 0,
    ONE_ERROR_CONNECTION_RECEIVE_BEFORE_SEND = 1,
    ONE_ERROR_CONNECTION_HELLO_SEND_FAILED = 2,
    ONE_ERROR_CONNECTION_HELLO_RECEIVE_FAILED = 3,
    ONE_ERROR_CONNECTION_HELLO_VERSION_MISMATCH = 4,
    ONE_ERROR_CONNECTION_HELLO_TOO_BIG = 5,
    ONE_ERROR_CONNECTION_HELLO_INVALID = 6,
    ONE_ERROR_CONNECTION_HELLO_MESSAGE_SEND_FAILED = 7,
    ONE_ERROR_CONNECTION_HELLO_MESSAGE_RECEIVE_FAILED = 8,
    ONE_ERROR_CONNECTION_HELLO_MESSAGE_HEADER_TOO_BIG = 9,
    ONE_ERROR_CONNECTION_HELLO_MESSAGE_REPLY_INVALID = 10,
    ONE_ERROR_CONNECTION_UPDATE_AFTER_ERROR = 11,
    ONE_ERROR_CONNECTION_UPDATE_READY_FAIL = 12,
    ONE_ERROR_CONNECTION_SEND_FAIL = 13,
    ONE_ERROR_CONNECTION_RECEIVE_FAIL = 14
};

inline bool is_error(Error err) {
    return err != ONE_ERROR_NONE;
}

const char *error_text(Error);

#ifdef __cplusplus
};
#endif
