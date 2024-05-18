#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "logs.h"

static FILE* __LOG_STREAM__ = nullptr;

static const char* EXTENSION = ".log.html";

// ------------------------------------------------------------

void OpenLogFile(const int argc, const char* argv[])
{
    bool is_flag = IsFlagOn(argc, argv, LOG_FLAG);
    if (!is_flag)
        return;

    const char* FILE_NAME = argv[0];

    char* file_name = strndup(FILE_NAME, MAX_FILE_NAME_LEN);

    __LOG_STREAM__ = fopen(strncat(file_name, EXTENSION, MAX_FILE_NAME_LEN + sizeof(EXTENSION)), "a");

    if (__LOG_STREAM__ == nullptr)
        __LOG_STREAM__ =  stderr;

    time_t now = 0;
    time(&now);

    srand(time(NULL));

    fprintf(__LOG_STREAM__, "<br>\n******************************************************************************<br>\n"
                              "=========================== PROGRAM START ===========================<br>\n"
                              "******************************************************************************<br>\n"
                              "RUNNED AT %s<br>\n", ctime(&now));

    #pragma GCC diagnostic ignored "-Wundef"
    #if CANARY_PROTECT
        fprintf(__LOG_STREAM__, "[CANARY PROTECT ON]<br>\n");
    #endif

    #if HASH_PROTECT
        fprintf(__LOG_STREAM__, "[HASH PROTECT ON]<br>\n");
    #endif
    #pragma GCC diagnostic warning "-Wundef"

    fprintf(__LOG_STREAM__, "<br>\n");

    atexit(CloseLogFile);
    free(file_name);
}

// ------------------------------------------------------------

bool IsFlagOn(const int argc, const char* argv[], const char* flag)
{
    if (argc > 1)
    {
        for (size_t i = 1; i < argc; i++)
        {
            if (!strncmp(flag, argv[i], MAX_FILE_NAME_LEN))
                return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------------------------------

void CloseLogFile()
{
    fprintf(__LOG_STREAM__, "******************************************************************************<br>\n"
                            "============================ PROGRAM END ============================<br>\n"
                            "******************************************************************************<br>\n");
    fclose(__LOG_STREAM__);
}

//-----------------------------------------------------------------------------------------------------

int LogDump(dump_f dump_func, const void* stk, const char* func, const char* file, const int line)
{
    if (__LOG_STREAM__ == nullptr)
        return 0;

    assert(dump_func);
    assert(stk);

    return dump_func(__LOG_STREAM__, stk, func, file, line);
}

//-----------------------------------------------------------------------------------------------------

int PrintLog (const char *format, ...)
{
    if (__LOG_STREAM__ == nullptr)
        return 0;

    va_list arg;
    int done;

    va_start (arg, format);
    done = vfprintf(__LOG_STREAM__, format, arg);
    va_end (arg);

    return done;
}

