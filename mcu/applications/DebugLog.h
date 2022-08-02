/*
 * UARTService.h
 *
 *  Author: thevinh
 */

#ifndef DEBUG_LOG_H_
#define DEBUG_LOG_H_

#include <stdarg.h>

#define LOG_LEVEL 3

enum LogLevel
{
    LOG_ERROR = 0,
    LOG_WARNING,
    LOG_HIGH,
    LOG_MID,
    LOG_LOW,
    LOG_INFO,
    LOG_VERBOSE
};

void debugLog(enum LogLevel lv, const char *__fmt, ...);

void debugLog_P(enum LogLevel lv, const char *__fmt, ...);

#endif /* DEBUG_LOG_H_ */