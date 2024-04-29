#ifndef __LOG_FUNCS_H_
#define __LOG_FUNCS_H_

#include <stdio.h>

#include "types.h"

/*! \file
* \brief Contains log functions
*/

static const size_t MAX_FILE_NAME_LEN = 100;

/************************************************************//**
 * @brief Opens log file, also close it when program shuts down
 *
 * @param[in] FILE_NAME name of log file
 ************************************************************/
void OpenLogFile(const char* FILE_NAME);

/************************************************************//**
 * @brief Closes log file
 ************************************************************/
void CloseLogFile();

/************************************************************//**
 * @brief Dumping information in logs
 *
 * @param[in] dump_func dumping function
 * @param[in] obj dumping object
 * @param[in] func function
 * @param[in] file file
 * @param[in] line line
 * @return int 0
 ************************************************************/
int LogDump(dump_f dump_func, const void* obj, const char* func, const char* file, const int line);

/************************************************************//**
 * @brief Prints text in log (printf analogue)
 *
 * @param[in] format text format (like in printf)
 * @param[in] ... extra arguments
 * @return int amount of written symbols
 *************************************************************/
int PrintLog (const char *format, ...);

#ifdef LOG_START
#undef LOG_START

#endif
#define LOG_START(func, file, line)         do                                                              \
                                            {                                                               \
                                                    PrintLog(                                               \
                                                    "--------------------LOGS CALLED--------------------<br>\n" \
                                                    "RUNNING FUNCTION %s FROM FILE \"%s\"(%d)<br>\n",           \
                                                    func, file, line);                                      \
                                            } while (0)


#ifdef LOG_END
#undef LOG_END

#endif
#define LOG_END()           do                                                                      \
                            {                                                                       \
                                PrintLog("END TIME: %s<br>\n"                                           \
                                        "--------------------------------------------------<br>\n",     \
                                        __TIME__);                                                  \
                            } while(0)

#ifdef LOG_SEPARATOR
#undef LOG_SEPARATOR

#endif
#define LOG_SEPARATOR()     PrintLog("<br>\n........................................<br>\n<br>\n");

#ifdef LOG_START_DUMP
#undef LOG_START_DUMP

#endif
#define LOG_START_DUMP(func, file, line)     do                                                             \
                                            {                                                               \
                                                    PrintLog(                                               \
                                                    "--------------------DUMP CALLED--------------------<br>\n" \
                                                    "RUNNING FUNCTION %s FROM FILE \"%s\"(%d)<br>\n",           \
                                                    func, file, line);                                      \
                                            } while (0)

#endif
