/*
 * main.c
 *
 * Created: 7/22/2022 7:24:57 PM
 *  Author: thevinh
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "thread.h"
#include "UARTService.h"
#include "DebugLog.h"
#include "stddef.h"
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

void* RunT1(void *arg)
{
    for (uint8_t j = 0; j < 2; j++)
    {
        uint16_t counter = 0;
        for (uint16_t i = 0; i < 30000; i++)
        {
            counter++;
            PORTC ^= (1 << PORTC1);
        }
    }
    return arg;
}

void* RunT2(void *arg)
{
    while (1)
    {
        uint16_t counter = 0;
        for (uint16_t i = 0; i < 30000; i++)
        {
            uint16_t c=0;
            for (uint16_t j = 0; j < 100; j++)
            {
                c++;
                PORTC ^= (1 << PORTC2);
            }
            counter++;
            debugLog_P(LOG_MID, PSTR("%s:%d\n"), __func__, counter);
        }
    }
    return arg;
}

void* RunT3(void *arg)
{
    while (1)
    {
        uint16_t counter = 0;
        for (uint16_t i = 0; i < 30000; i++)
        {
            counter++;
            PORTC ^= (1 << PORTC3);
        }
    }
    return arg;
}

int main(void)
{
    __malloc_heap_end =(uint16_t) 1800;
    DDRB |= 0b11111111;
    DDRC |= 0b00001111;
    init_mutil_thread();
    UART_Init(UART_UBRR);
    sei();
    PORTC ^= (1 << PORTC0);
    debugLog_P(LOG_HIGH, PSTR("Program start\n"));

    // thread_t t1_id;
    thread_t t2_id;
    // thread_t t3_id;
    // void *return_t1;
    // void *return_t2;
    // void *return_t3;

    // thread_create(&t1_id, RunT1, NULL);
    thread_create(&t2_id, RunT2, NULL);
    // thread_create(&t3_id, RunT3, NULL);

    // thread_join(t1_id, &return_t1);
    // thread_join(t2_id, &return_t2);
    // thread_join(t3_id, &return_t2);
    while (1)
    {
        // TODO:: Please write your application code
        uint16_t counter = 0;
        for (uint16_t i = 0; i < 1000; i++)
        {
            uint16_t c=0;
            for (uint16_t j = 0; j < 123; j++)
            {
                c++;
                PORTC ^= (1 << PORTC0);
            }
            counter++;
            debugLog_P(LOG_MID, PSTR("%s:%d\n"), __func__, counter);
            for ( uint8_t i = MIN_THREAD_ID; i <= MIN_THREAD_ID + NUM_OF_THREAD; ++i)
            {
                uint8_t status = get_thread_status(i);
                uint8_t stackSize = 0;
                if (status == 1)
                {
                    stackSize = get_thread_stack_use_size(i);
                }
                debugLog(LOG_MID, "%s:id[%d]sts[%d]Siz[%d]\n", __func__, i, status, stackSize);
            }
            PORTC ^= (1 << PORTC0);
        }
    }
}