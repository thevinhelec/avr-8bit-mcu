/*
 * my_uart.c
 *
 * Created: 12/17/2021 7:27:03 PM
 *  Author: thevi
 */
#include "UARTService.h"
#include "uart.h"
#include "thread.h"
#include "util/handler.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static volatile mt_lock_t trans_lock = THREAD_UNLOCK;
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
    //UART_printf_sync("UARTService::%s:\n",__func__ );
    struct UARTMessage *uartMessage = (struct UARTMessage *)message;
    UART_sendBytes_sync(uartMessage->buffer, uartMessage->size);
    free(uartMessage->buffer);
    free(uartMessage);
}

void UART_Init(uint16_t ubrr)
{
    uart_init(ubrr);
    setupCqueue(&message_queue, &message_buffer, UART_MESSAGE_QUEUE_SIZE); // setup message queue
    uint8_t status = handler_create(&uart_handler, onMessageReceive, &message_queue);
    if (status != SUCCESS)
    {
        UART_printf_sync("UARTService::%s:ERROR: Can not create handler\n",__func__ );
    }
}

void UART_sendBytes_sync(const uint8_t *buffer, const uint8_t size)
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
            thread_sleep(8); // time to send 32 frame / 4
    }
    thread_mutex_unlock(&trans_lock);
}

void UART_sendBytes(const uint8_t *buffer, const uint8_t size)
{
    struct UARTMessage *message = (struct UARTMessage *)malloc(sizeof(struct UARTMessage));

    uint8_t *bufferPtr = (uint8_t *)malloc(size);
    memcpy(bufferPtr, buffer, size);

    message->size = size;
    message->buffer = bufferPtr;

    uint8_t status = addMessage(&uart_handler, message);
    if (status != SUCCESS)
    {
        free(bufferPtr);
        free(message);
        UART_printf_sync("UARTService::%s:ERROR: Can not add message\n",__func__ );
    }
}

/*
Replace \\n = \\r

return postion of null character
*/
void editCharacters(uint8_t *buffer, uint8_t *null_position)
{
    for (uint8_t i = 0; i < UART_PRINT_MAX_SIZE - 1; ++i)
    {
        uint8_t *c = &buffer[i];
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

void UART_print_sync(char *buffer)
{
    uint8_t null_pos;
    editCharacters(buffer, &null_pos);
    UART_sendBytes_sync(buffer, null_pos + 1);
}

void UART_printf_sync(const char *__fmt, ...)
{
    va_list arguments;

    /* Initializing arguments to store all values after __fmt */
    va_start(arguments, __fmt);
    char buffer[UART_PRINT_MAX_SIZE];
    vsprintf(buffer, __fmt, arguments);
    UART_print_sync(buffer);
    va_end(arguments);
}

void UART_print(char *buffer)
{
    uint8_t null_pos;
    editCharacters(buffer, &null_pos);
    UART_sendBytes(buffer, null_pos + 1);
}

void UART_printf(const char *__fmt, ...)
{
    va_list arguments;

    /* Initializing arguments to store all values after __fmt */
    va_start(arguments, __fmt);
    char buffer[UART_PRINT_MAX_SIZE];
    vsprintf(buffer, __fmt, arguments);
    UART_print(buffer);
    va_end(arguments);
}
