/*
 * main.c
 *
 * Created: 7/22/2022 7:24:57 PM
 *  Author: thevinh
 */

#include <avr/io.h>
#include "thread.h"
#include "UARTService.h"
#include "stddef.h"
#include <stdio.h>

void *RunT1(void *arg)
{
    for (uint8_t j = 0; j < 2; j++)
    {
        uint16_t counter = 0;
        //UART_print("print test in T1\n");
        for (uint16_t i = 0; i < 30000; i++)
        {
            counter++;
            PORTB ^= (1 << PORTB1);
        }
    }
    return arg;
}

void *RunT2(void *arg)
{
    while (1)
    {
        // char buffer[15];
        // sprintf(buffer, "j = %d\r", j);
        // UART_sendBytes( (uint8_t*) buffer, 14);
        uint16_t counter = 0;
        for (uint16_t i = 0; i < 10000; i++)
        {
            uint16_t c=0;
            for (uint16_t j = 0; j < 10000; j++)
            {
                c++;
            }
            counter++;
            UART_printf_sync("main::%s: %d:\n",__func__, counter);
            PORTB ^= (1 << PORTB2);
        }
        //UART_printf("hhhh\n");
        //UART_printf("main::%s:T2 thread runing\n",__func__);
        //UART_print("print test in T2\n");
        
            // char buffer[15];
            // sprintf(buffer, "%dtt%d\r", get_status(thread_self()), thread_self());
            // UART_sendBytes( (uint8_t*) buffer, 14);
        
    }
    return arg;
}

void *RunT3(void *arg)
{
    while (1)
    {
        uint16_t counter = 0;
        //UART_print("print test in T3\n");
        for (uint16_t i = 0; i < 30000; i++)
        {
            counter++;
            PORTB ^= (1 << PORTB3);
        }
    }
    return arg;
}

int main(void)
{
    DDRB |= 0b00001111;
    init_mutil_thread();
    sei();
    UART_Init(MYUBRR);
    UART_print_sync("Program start\n");

    thread_t t1_id;
    thread_t t2_id;
    thread_t t3_id;
    void *return_t1;
    void *return_t2;
    void *return_t3;

    // thread_create(&t1_id, RunT1, NULL);
    thread_create(&t2_id, RunT2, NULL);
    // thread_create(&t3_id, RunT3, NULL);

    // char buffer[15];
    // sprintf(buffer, "%d-%d-%d\r", t1_id, t2_id, t3_id);
    // UART_sendBytes( (uint8_t*) buffer, 14);

    // thread_join(t1_id, &return_t1);
    // thread_join(t2_id, &return_t2);
    // thread_join(t3_id, &return_t2);
    while (1)
    {
        // TODO:: Please write your application code
        // thread_sleep(10);
        uint16_t counter = 0;
        for (uint16_t i = 0; i < 10000; i++)
        {
            uint16_t c=0;
            for (uint16_t j = 0; j < 10000; j++)
            {
                c++;
            }
            counter++;
            //UART_printf("main::%s: %d:\n",__func__, counter);
            PORTB ^= (1 << PORTB0);
        }
        //UART_printf("main::%s:main thread runing\n",__func__);
    }
}