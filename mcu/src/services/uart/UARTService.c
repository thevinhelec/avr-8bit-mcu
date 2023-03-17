/*
 * UARTService.c
 *
 *  Author: thevinh
 */

#include "services/uart/UARTService.h"
#include "drivers/uart/uart.h"
#include "drivers/thread/thread.h"
#include "utils/handler.h"
#ifdef UART_ASYNC
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#endif
#include <stdio.h>
#include <stddef.h>

#ifdef UART_ASYNC
#define UART_MESSAGE_QUEUE_SIZE 5
#endif
#define UART_PRINT_MAX_SIZE 48

static volatile mt_lock_t trans_lock = THREAD_UNLOCK;
#ifdef UART_ASYNC
static struct Handler uart_handler;
static volatile uint16_t message_buffer[UART_MESSAGE_QUEUE_SIZE];
static volatile struct Cqueue message_queue;

struct UARTMessage
{
    uint8_t size;
    uint8_t *buffer;
};

void onMessageReceive(void *message)
{
    struct UARTMessage *uartMessage = (struct UARTMessage *)message;
    UART_sendBytes(uartMessage->buffer, uartMessage->size);
    free(uartMessage->buffer);
    free(uartMessage);
}
#endif

void UART_Init(uint16_t ubrr)
{
    uart_init(ubrr);
#ifdef UART_ASYNC
    setupCqueue(&message_queue, &message_buffer, UART_MESSAGE_QUEUE_SIZE); // setup message queue
    uint8_t status = handler_create(&uart_handler, onMessageReceive, &message_queue);
    if (status != SUCCESS)
    {
        UART_printf_P(PSTR("%s:handler_create fails\n"), __func__);
    }
#endif
}

void UART_sendBytes(const uint8_t *buffer, const uint8_t size)
{
    uint8_t send_size = size;
    uint8_t i = 0;
    thread_mutex_lock(&trans_lock);
    while (send_size > 0)
    {
        uint8_t send_byte = write_uart(&buffer[i], send_size);
        i = send_byte + i;
        send_size = send_size - send_byte;
        if (send_size > 0)
            thread_sleep(80);
    }
    thread_mutex_unlock(&trans_lock);
}

void UART_sendBytes_async(const uint8_t *buffer, const uint8_t size)
{
#if defined(UART_ASYNC)
    struct UARTMessage *message = (struct UARTMessage *)malloc(sizeof(struct UARTMessage));
    uint8_t *bufferPtr = (uint8_t *)malloc(size);

    if (message == NULL || bufferPtr == NULL)
    {
        UART_printf_P(PSTR("%s:malloc fails\n"), __func__);
        free(bufferPtr);
        free(message);
        return;
    }

    memcpy(bufferPtr, buffer, size);
    message->size = size;
    message->buffer = bufferPtr;

    uint8_t status = addMessage(&uart_handler, message);
    if (status != SUCCESS)
    {
        UART_printf_P(PSTR("%s:addMessage fails\n"), __func__);
        free(bufferPtr);
        free(message);
    }
#else
    uint8_t send_size = size;
    uint8_t i = 0;
    thread_mutex_lock(&trans_lock);
    while (send_size > 0)
    {
        uint8_t send_byte = write_uart(&buffer[i], send_size);
        i = send_byte + i;
        send_size = send_size - send_byte;
        if (send_size > 0)
            thread_sleep(80);
    }
    thread_mutex_unlock(&trans_lock);
#endif
}

/*
Replace \\n = \\r
return postion of null character
*/
void editCharacters(char *buffer, uint8_t *null_position)
{
    for (uint8_t i = 0; i < UART_PRINT_MAX_SIZE - 1; ++i)
    {
        char *c = &buffer[i];
        if (*c == '\0')
        {
            *null_position = i;
            return;
        }
        if (*c == '\n')
        {
            *c = '\r';
            continue;
        }
    }
    buffer[UART_PRINT_MAX_SIZE - 1] = '\0';
    *null_position = UART_PRINT_MAX_SIZE - 1;
}

void UART_print(char *buffer)
{
    uint8_t null_pos;
    editCharacters(buffer, &null_pos);
    UART_sendBytes(buffer, null_pos + 1);
}

void UART_print_async(char *buffer)
{
    uint8_t null_pos;
    editCharacters(buffer, &null_pos);
#if defined(UART_ASYNC)
    UART_sendBytes_async(buffer, null_pos + 1);
#else
    UART_sendBytes(buffer, null_pos + 1);
#endif
}

void UART_printf(const char *__fmt, ...)
{
    va_list arguments;

    /* Initializing arguments to store all values after __fmt */
    va_start(arguments, __fmt);
    char buffer[UART_PRINT_MAX_SIZE];
    vsnprintf(buffer, UART_PRINT_MAX_SIZE, __fmt, arguments);
    UART_print(buffer);
    va_end(arguments);
}

void UART_vprintf(const char *__fmt, va_list args)
{
    char buffer[UART_PRINT_MAX_SIZE];
    vsnprintf(buffer, UART_PRINT_MAX_SIZE, __fmt, args);
    UART_print(buffer);
}

void UART_printf_async(const char *__fmt, ...)
{
    va_list arguments;

    /* Initializing arguments to store all values after __fmt */
    va_start(arguments, __fmt);
    char buffer[UART_PRINT_MAX_SIZE];
    vsnprintf(buffer, UART_PRINT_MAX_SIZE, __fmt, arguments);

#if defined(UART_ASYNC)
    UART_print_async(buffer);
#else
    UART_print(buffer);
#endif
    va_end(arguments);
}

void UART_printf_P(const char *__fmt_P, ...)
{
    va_list arguments;
    /* Initializing arguments to store all values after __fmt */
    va_start(arguments, __fmt_P);
    char buffer[UART_PRINT_MAX_SIZE];
    vsnprintf_P(buffer, UART_PRINT_MAX_SIZE, __fmt_P, arguments);
    UART_print(buffer);
    va_end(arguments);
}

void UART_vprintf_P(const char *__fmt_P, va_list args)
{
    char buffer[UART_PRINT_MAX_SIZE];
    vsnprintf_P(buffer, UART_PRINT_MAX_SIZE, __fmt_P, args);
    UART_print(buffer);
}

void UART_printf_async_P(const char *__fmt_P, ...)
{
    va_list arguments;

    /* Initializing arguments to store all values after __fmt */
    va_start(arguments, __fmt_P);
    char buffer[UART_PRINT_MAX_SIZE];
    vsnprintf_P(buffer, UART_PRINT_MAX_SIZE, __fmt_P, arguments);

#if defined(UART_ASYNC)
    UART_print_async(buffer);
#else
    UART_print(buffer);
#endif
    va_end(arguments);
}