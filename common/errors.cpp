#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "errors.h"
#include "logs.h"

static const size_t MAX_ERROR_LEN = 200;

int PrintError(FILE* fp, const void* err, const char* func, const char* file, const int line)
{
    assert(err);

    LOG_START(func, file, line);

    #pragma GCC diagnostic ignored "-Wcast-qual"
    struct ErrorInfo* error = (struct ErrorInfo*) err;

    switch ((ERRORS) error->code)
    {
        case (ERRORS::NONE):
            LOG_END();
            return (int) error->code;

        case (ERRORS::OPEN_FILE):
            fprintf(fp, "OPEN FILE ERROR<br>\n"
                        "FAILED TO OPEN FILE \"%s\"<br>\n", (char*) error->data);
            LOG_END();
            return (int) error->code;

        case (ERRORS::READ_FILE):
            fprintf(fp, "READ FILE ERROR<br>\n"
                        "FAILED TO READ INFO FROM FILE \"%s\"<br>\n", (char*) error->data);
            LOG_END();
            return (int) error->code;

        case (ERRORS::ALLOCATE_MEMORY):
            fprintf(fp, "MEMORY ALLOCATE ERROR<br>\n"
                        "FAILED TO ALLOCATE MEMORY IN \"%s\"<br>\n", (char*) error->data);
            LOG_END();
            return (int) error->code;

        case (ERRORS::INVALID_STACK):
            fprintf(fp, "ERROR WITH STACK<br>\n");
            LOG_END();
            return (int) error->code;

        case (ERRORS::PRINT_DATA):
            fprintf(fp, "DATA PRINT ERROR<br>\n"
                        "FAILED TO PRINT DATA IN \"%s\"<br>\n", (char*) error->data);
            LOG_END();
            return (int) error->code;

        case (ERRORS::USER_QUIT):
            fprintf(fp, "USER DECIDED TO QUIT<br>\n");
            LOG_END();
            return (int) error->code;

        case (ERRORS::UNKNOWN):
        // fall through
        default:
            fprintf(fp, "UNKNOWN ERROR<br>\n");
            LOG_END();
            return (int) ERRORS::UNKNOWN;
    }

    #pragma GCC diagnostic warning "-Wcast-qual"
}

//-----------------------------------------------------------------------------------------------------

int SetErrorData (error_t* error, const char *format, ...)
{
    assert(error);

    char* data = (char*) calloc(MAX_ERROR_LEN, sizeof(char));

    assert(data);

    va_list arg;
    int done;

    va_start (arg, format);
    done = vsnprintf(data, MAX_ERROR_LEN, format, arg);
    va_end (arg);

    error->data = data;

    return done;
}
