#include <string.h>

#include "frontend.h"

//-----------------------------------------------------------------------------------------------------

int PrintFrontendError(FILE* fp, const void* err, const char* func, const char* file, const int line)
{
    assert(err);

    LOG_START(func, file, line);

    const struct ErrorInfo* error = (const struct ErrorInfo*) err;

    switch ((FrontendErrors) error->code)
    {
        case (FrontendErrors::NONE):
            LOG_END();
            return (int) error->code;

        case (FrontendErrors::INVALID_SYNTAX):
            fprintf(fp, "INVALID SYNTAX:<br>\n"
                        "%s<br>\n", (const char*) error->data);
            LOG_END();
            return (int) FrontendErrors::UNKNOWN;

        case (FrontendErrors::UNKNOWN_OPERATOR):
            fprintf(fp, "UNKNOWN_OPERATOR:<br>\n"
                        "%s<br>\n", (const char*) error->data);
            LOG_END();
            return (int) FrontendErrors::UNKNOWN;

        case (FrontendErrors::UNKNOWN):
        // fall through
        default:
            fprintf(fp, "UNKNOWN FRONTEND ERROR<br>\n");
            LOG_END();
            return (int) FrontendErrors::UNKNOWN;
    }
}
