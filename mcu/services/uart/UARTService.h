/*
 * UARTService.h
 *
 *  Author: thevinh
 */

#ifndef UART_SERVICE_H_
#define UART_SERVICE_H_

#include <stdint.h>
#include <stdarg.h>
#include <avr/pgmspace.h>
#include "bsp.h"

#define UART_BAUD 4800 // Baudrate
#define UART_UBRR UART_FOSC / 16 / UART_BAUD - 1

/*
Initialize UART, transmitter queue
parameter UBRR, factor to set baud rate
return void
*/
void UART_Init(uint16_t ubrr);

/*
Put bytes data in buffer to UART transmitter queue, this function block until
all data is put to queue.  This function is thread-safe.
parameter buffer : pointer to input data
          size : size of data to send. max 255 bytes
return void
*/
void UART_sendBytes(const uint8_t *buffer, const uint8_t size);

/*
Same as UART_sendBytes_sync(), but this function is async.
parameter buffer : pointer to input data
          size : size of data to send. max 255 bytes
return void
*/
void UART_sendBytes_async(const uint8_t *buffer, const uint8_t size);

/*
Put text data in buffer to UART transmitter queue, terminates when encounter a null character (\0).
A null character will be add to transmitter queue if not  encounter a null character.
this function block until all data is put to transmitter queue. This function is thread-safe.
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_print(char *buffer);

/*
Same as UART_print(), but this function is async.
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_print_async(char *buffer);

/*
Put format text data to UART. Use same as printf.
This function block until all data is put to queue. This function is thread-safe.
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_printf(const char *__fmt_P, ...);

/*
Same as UART_printf(), but this function is async.
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_printf_async(const char *__fmt_P, ...);

/*
Same as UART_printf(), but use program memory data
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_printf_P(const char *__fmt_P, ...);

/*
Same as UART_printf_async(), but use program memory data
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_printf_async_P(const char *__fmt, ...);

#endif /* UART_SERVICE_H_ */