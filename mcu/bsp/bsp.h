/*
 * uart.c
 *
 *  Author: thevinh
 */
#ifndef BSP_H_
#define BSP_H_
/* config for atmega328p mcu*/
// UART config begin
#if (USE_UART_ASYNC == 1)
#define UART_ASYNC
#endif
// UART config end

#define UART_FOSC 1000000 // Clock Speed

// Thread config begin
#define TIMER0_OVF_vect_ADD "0x00000040"
// Thread config end

#endif BSP_H_
