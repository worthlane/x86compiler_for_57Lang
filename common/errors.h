#ifndef __ERRORS_H
#define __ERRORS_H

/*! \file
* \brief Contains enumerated errors
*/

#include <stdio.h>
#include "logs.h"

#ifdef EXIT_IF_ERROR
#undef EXIT_IF_ERROR
#endif
#define EXIT_IF_ERROR(error)                do                                                          \
                                            {                                                           \
                                                if ((error)->code != (int) ERRORS::NONE)                \
                                                {                                                       \
                                                    return LogDump(PrintError, error, __func__,         \
                                                                    __FILE__, __LINE__);                \
                                                }                                                       \
                                            } while(0)

#ifdef BREAK_IF_ERROR
#undef BREAK_IF_ERROR
#endif
#define BREAK_IF_ERROR(error)                do                                                          \
                                            {                                                           \
                                                if ((error)->code != (int) ERRORS::NONE)                \
                                                {                                                       \
                                                    return;                                             \
                                                }                                                       \
                                            } while(0)
#ifdef RETURN_IF_ERROR
#undef RETURN_IF_ERROR
#endif
#define RETURN_IF_ERROR(error)              do                                                          \
                                            {                                                           \
                                                if ((error) != ERRORS::NONE)                            \
                                                {                                                       \
                                                    return error;                                       \
                                                }                                                       \
                                            } while(0)

#ifdef RETURN_IF_NOT_EQUAL
#undef RETURN_IF_NOT_EQUAL
#endif
#define RETURN_IF_NOT_EQUAL(check_err, expected_err, return_err)                            \
                                                                                            \
                            do                                                              \
                            {                                                               \
                                if ((check_err) != (expected_err))                          \
                                {                                                           \
                                    return (return_err);                                    \
                                }                                                           \
                            } while(0)


/// @brief list of errors
enum class ERRORS
{
    /// not an error
    NONE = 0,

    /// error while opening file
    OPEN_FILE,
    /// error while reading file
    READ_FILE,

    /// error with memory allocating
    ALLOCATE_MEMORY,

    /// error while printing data
    PRINT_DATA,

    USER_QUIT,

    INVALID_STACK,

    INVALID_IR,

    /// unknown error
    UNKNOWN
};

/// @brief struct with all error info
struct ErrorInfo
{
    /// error code
    int code;
    /// error data
    const void* data;
};

typedef struct ErrorInfo error_t;

/************************************************************//**
 * @brief Prints error from error list
 *
 * @param[in] fp output stream
 * @param[in] error error
 * @param[in] func function where error happened
 * @param[in] file file where error happened
 * @param[in] line line where error happened
 * @return int error code
 ************************************************************/
int PrintError(FILE* fp, const void* error, const char* func, const char* file, const int line);

int SetErrorData (error_t* error, const char *format, ...);

#endif
