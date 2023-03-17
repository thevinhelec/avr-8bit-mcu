/*
 * uart.c
 *
 *  Author: thevinh
 */
#include <avr/interrupt.h>
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
#if (defined __AVR_ATmega2560__)
#define THREAD_TIMER_OVF_VECT TIMER0_OVF_vect
#define THREAD_TIMER_OVF_VECT_ADDRESS "0x0000005C"
#elif ((defined __AVR_ATmega328__ || __AVR_ATmega328__))
#define THREAD_TIMER_OVF_VECT TIMER0_OVF_vect
#define THREAD_TIMER_OVF_VECT_ADDRESS "0x00000040"
#endif
// Thread config end

#endif BSP_H_
