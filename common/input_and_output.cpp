#include <ctype.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include "input_and_output.h"
#include "colorlib.h"

static void ReadLine(FILE* fp, char* buf);

//-----------------------------------------------------------------------------------------------------

void SkipSpaces(FILE* fp)
{
    char ch = 0;
    ch = getc(fp);

    while (isspace(ch))
        ch = getc(fp);

    ungetc(ch, fp);
}

//------------------------------------------------------------------------------------------------------------------

void ClearInput(FILE* fp)
{
    int ch = 0;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {}
}

//-----------------------------------------------------------------------------------------------------

char* GetDataFromLine(FILE* fp, error_t* error)
{
    assert(error);

    char* line = (char*) calloc(MAX_STRING_LEN, sizeof(char));
    if (line == nullptr)
    {
        error->code = (int) ERRORS::ALLOCATE_MEMORY;
        return nullptr;
    }

    ReadLine(fp, line);

    return line;
}

//-----------------------------------------------------------------------------------------------------

static void ReadLine(FILE* fp, char* buf)
{
    assert(buf);

    for (size_t i = 0; i < MAX_STRING_LEN; i++)
    {
        char ch = getc(fp);

        if (ch == EOF)
            break;

        if (ch == '\n' || ch == '\0')
        {
            buf[i] = 0;
            break;
        }
        else
            buf[i] = ch;
    }
}

//-----------------------------------------------------------------------------------------------------

bool DoesLineHaveOtherSymbols(FILE* fp)
{
    int ch = 0;

    while ((ch = getc(fp)) != '\n' && ch != EOF)
    {
        if (!isspace(ch))
            {
                ClearInput(fp);
                return true;
            }
    }

    return false;
}

//-----------------------------------------------------------------------------------------------------

FILE* OpenInputFile(const char* file_name, error_t* error)
{
    assert(file_name);
    assert(error);

    FILE* fp = fopen(file_name, "r");
    if (!fp)
    {
        error->code = (int) ERRORS::OPEN_FILE;
        error->data = file_name;
    }

    return fp;
}

//-----------------------------------------------------------------------------------------------------

FILE* OpenOutputFile(const char* file_name, error_t* error)
{
    assert(file_name);
    assert(error);

    FILE* fp = fopen(file_name, "w");
    if (!fp)
    {
        error->code = (int) ERRORS::OPEN_FILE;
        error->data = file_name;
    }

    return fp;
}

//-----------------------------------------------------------------------------------------------------

FILE* OpenFile(const char* file_name, const char* mode, error_t* error)
{
    assert(file_name);
    assert(error);
    assert(mode);

    FILE* fp = fopen(file_name, mode);
    if (!fp)
    {
        error->code = (int) ERRORS::OPEN_FILE;
        error->data = file_name;
    }

    return fp;
}

//-----------------------------------------------------------------------------------------------------

const char* GetFileName(const int argc, const char* argv[], const int id, const char* mode, error_t* error)
{
    assert(error);
    assert(argv);

    const char* file_name = nullptr;

    if (argc > id)
        file_name = argv[id];
    else
    {
        PrintGreenText(stdout, "ENTER %s FILE NAME: \n", mode);
        file_name = GetDataFromLine(stdin, error);
    }

    if (file_name != nullptr)
        PrintGreenText(stdout, "%s FILE NAME: \"%s\"\n", mode, file_name);

    return file_name;
}
