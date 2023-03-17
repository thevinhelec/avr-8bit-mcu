/*
 * uart.h
 *
 *  Author: thevinh
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

#define UART_TRANSMITTER_QUEUE_SIZE 96

/*
initialize UART, transmitter queue
parameter UBRR, factor to set baud rate
return void
*/
void uart_init(uint16_t ubrr);

/*
put bytes data in buffer to transmitter queue.
parameter buffer : pointer to input data
          size : size of data to send. max 255 bytes
return uint8_t : number of bytes had send
*/
uint8_t write_uart(const uint8_t *buffer, uint8_t size);

#endif /* UART_H_ */