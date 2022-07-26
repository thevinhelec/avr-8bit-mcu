/*
 * my_uart.h
 *
 * Created: 12/12/2021 7:55:55 PM
 *  Author: thevi
 */

#ifndef UART_SERVICE_H_
#define UART_SERVICE_H_

#include <stdint.h>
#include <stdarg.h>

#define FOSC 1000000 // Clock Speed
#define BAUD 4800
#define MYUBRR FOSC/16/BAUD-1
#define UART_MESSAGE_QUEUE_SIZE 10
#define UART_PRINT_MAX_SIZE 32

/*
initialize UART, transmitter queue
parameter UBRR, factor to set baud rate
return void
*/
void UART_Init(uint16_t ubrr);

/*
put bytes data in buffer to UART transmitter queue, this function block until
all data is put to queue.  This function is thread-safe.
parameter buffer : pointer to input data
          size : size of data to send. max 255 bytes
return void
*/
void UART_sendBytes_sync(const uint8_t *buffer, const uint8_t size);

/*
put bytes data in buffer to transmitter queue. This function is async.  This function is thread-safe.
parameter buffer : pointer to input data
          size : size of data to send. max 255 bytes
return void
*/
void UART_sendBytes(const uint8_t *buffer, const uint8_t size);

/*
put text data in buffer to UART transmitter queue, terminates when encounter a null character (\0).
A null character will be add to transmitter queue if not  encounter a null character.
this function block until all data is put to queue. This function is thread-safe.
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_print_sync(char *buffer);

/*
format text data to UART. same as printf
this function block until all data is put to queue. This function is thread-safe.
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_printf_sync(const char *__fmt, ...);

/*
put text data in buffer to transmitter queue, terminates when encounter a null character (\0).
A null character will be add to transmitter queue if not  encounter a null character.
This function is async. This function is thread-safe.
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_print(char *buffer);

/*
print format text data to UART. same as printf
A null character will be add to transmitter queue if not  encounter a null character.
This function is async. This function is thread-safe.
parameter buffer : pointer to input data, max size of buffer is 255
return void
*/
void UART_printf(const char *__fmt, ...);

#endif /* UART_SERVICE_H_ */