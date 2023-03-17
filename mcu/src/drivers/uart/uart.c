/*
 * uart.c
 *
 *  Author: thevinh
 */
#include "drivers/uart/uart.h"
#include <avr/interrupt.h>
#include "utils/circular_queue.h"

static uint16_t transmitter_buffer[UART_TRANSMITTER_QUEUE_SIZE];
static struct Cqueue transmitter_queue;

void USART_Transmit(const uint8_t data)
{
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    /* Put data into transmit buffer, sends the data */
    UDR0 = data;
}

#if (defined __AVR_ATmega2560__)
ISR(USART0_UDRE_vect)
#elif ((defined __AVR_ATmega328__ || __AVR_ATmega328__))
ISR(USART_UDRE_vect)
#endif
{ // USART Data Register Empty interrupt
    if (isEmpty(&transmitter_queue))
    {
        UCSR0B &= ~(1 << UDRIE0); // disable the data register empty interrupt
    }
    else
    {
        uint8_t data = dequeue(&transmitter_queue);
        USART_Transmit(data);
    }
}

void uart_init(uint16_t ubrr)
{
    #if (defined __AVR_ATmega2560__)
    DDRE |= 2;
    #elif ((defined __AVR_ATmega328__ || __AVR_ATmega328__))
    DDRD |= 2;
    #endif
                                                                        // Configure the Pin D1 as output
    setupCqueue(&transmitter_queue, &transmitter_buffer, UART_TRANSMITTER_QUEUE_SIZE); // setup transmitter queue
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    /*Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    /* Set frame format: 8data, 1stop bit, none Parity */
    UCSR0C = (3 << UCSZ00);
}

uint8_t write_uart(const uint8_t *buffer, uint8_t size)
{
    UCSR0B &= ~(1 << UDRIE0); // disable data register empty interrupt
    uint8_t free_size = getFreeSize(&transmitter_queue);
    uint8_t min = (free_size > size) ? size : free_size;
    for (uint8_t i = 0; i < min; i++)
    {
        enqueue(&transmitter_queue, buffer[i]);
    }
    UCSR0B |= (1 << UDRIE0); // enable data register empty interrupt
    return min;
}