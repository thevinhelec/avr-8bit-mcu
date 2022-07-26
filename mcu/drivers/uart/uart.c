/*
 * my_uart.c
 *
 * Created: 12/17/2021 7:27:03 PM
 *  Author: thevi
 */
#include "uart.h"
#include <avr/interrupt.h>
#include "util/circular_queue.h"

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

ISR(USART_UDRE_vect)
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
    DDRD |= 2;                                                                         // Configure the Pin D1 as output
    setupCqueue(&transmitter_queue, &transmitter_buffer, UART_TRANSMITTER_QUEUE_SIZE); // setup transmitter queue
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    /*Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    /* Set frame format: 8data, 1stop bit, none Parity */
    UCSR0C = (3 << UCSZ00);
}

uint8_t minimum(uint8_t a, uint8_t b)
{
    if (a > b)
        return b;
    return a;
}

uint8_t write_uart(const uint8_t *buffer, uint8_t size)
{
    UCSR0B &= ~(1 << UDRIE0); // disable data register empty interrupt
    uint8_t free_size = get_free_size(&transmitter_queue);
    uint8_t min = minimum((uint8_t)free_size, size);
    for (uint8_t i = 0; i < min; i++)
    {
        enqueue(&transmitter_queue, buffer[i]);
    }
    UCSR0B |= (1 << UDRIE0); // enable data register empty interrupt
    return min;
}
