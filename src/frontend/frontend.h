#ifndef __FRONTEND_H_
#define __FRONTEND_H_

#include "tree/tree.h"
#include "stack/nametable.h"

// ======================================================================
// ERRORS
// ======================================================================

enum class FrontendErrors
{
    NONE,

    INVALID_SYNTAX,
    UNKNOWN_OPERATOR,

    UNKNOWN
};

int PrintFrontendError(FILE* fp, const void* err, const char* func, const char* file, const int line);

#ifdef EXIT_IF_FRONTEND_ERROR
#undef EXIT_IF_FRONTEND_ERROR
#endif
#define EXIT_IF_FRONTEND_ERROR              do                                                            \
                                            {                                                           \
                                                if ((error).code != (int) FrontendErrors::NONE)      \
                                                {                                                       \
                                                    return LogDump(PrintFrontendError, &error, __func__,     \
                                                                    __FILE__, __LINE__);                \
                                                }                                                       \
                                            } while(0)
#ifdef RETURN_IF_FRONTEND_ERROR
#undef RETURN_IF_FRONTEND_ERROR
#endif
#define RETURN_IF_FRONTEND_ERROR            do                                                            \
                                            {                                                           \
                                                if (((error)->code) != (int) FrontendErrors::NONE)                  \
                                                {                                                       \
                                                    return (FrontendErrors) ((error)->code);                                       \
                                                }                                                       \
                                            } while(0)

#ifdef SET_FRONTEND_ERROR
#undef SET_FRONTEND_ERROR
#endif
#define SET_FRONTEND_ERROR(id, string, ...)                                                        \
                                            do                                                          \
                                            {                                                           \
                                                (error)->code = (int) FrontendErrors::id;               \
                                                SetErrorData(error, string, __VA_ARGS__);               \
                                                return (FrontendErrors) (error)->code;                  \
                                            } while(0)


#endif
