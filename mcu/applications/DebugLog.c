/*
 * UARTService.h
 *
 *  Author: thevinh
 */

#include "DebugLog.h"
#include "UARTService.h"

void debugLog(enum LogLevel lv, const char *__fmt, ...)
{
    if (lv > LOG_LEVEL) return;
    va_list arguments;
    /* Initializing arguments to store all values after __fmt */
    va_start(arguments, __fmt);
    UART_vprintf(__fmt, arguments);
    va_end(arguments);
}

void debugLog_P(enum LogLevel lv, const char *__fmt_P, ...)
{
    if (lv > LOG_LEVEL) return;
    va_list arguments;
    /* Initializing arguments to store all values after __fmt */
    va_start(arguments, __fmt_P);
    UART_vprintf_P(__fmt_P, arguments);
    va_end(arguments);
}