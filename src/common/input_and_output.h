#ifndef __INPUT_AND_OUTPUT_H_
#define __INPUT_AND_OUTPUT_H_

#include <stdio.h>

#include "errors.h"

static const size_t MAX_SIGNATURE_LEN = 100;
static const char* DEFAULT_SIGNATURE = "a.out";

static const size_t MAX_STRING_LEN  = 100;
static const size_t MAX_COMMAND_LEN = 200;

void SkipSpaces(FILE* fp);
void ClearInput(FILE* fp);

char* GetDataFromLine(FILE* fp, error_t* error);
bool DoesLineHaveOtherSymbols(FILE* fp);

const char* GetFileName(const int argc, const char* argv[], const int id, const char* mode, error_t* error);

FILE* OpenInputFile(const char* file_name, error_t* error);
FILE* OpenOutputFile(const char* file_name, error_t* error);

FILE* OpenFile(const char* file_name, const char* mode, error_t* error);

void PrintSignature(FILE* out_stream, const char* data_file);
const char* GetSignature(FILE* in_stream);

#endif
