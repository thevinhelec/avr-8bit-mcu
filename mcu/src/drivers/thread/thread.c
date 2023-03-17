/*
 * uart.c
 *
 *  Author: thevinh
 */

#include <drivers/thread/thread.h>
#include <stddef.h>
#include <string.h>
#include <avr/interrupt.h>
#include "bsp/bsp.h"

#define THREAD_BLOCK() \
    TIMSK0 = 0;        \
    asm("CLI \n"       \
        "CALL " THREAD_TIMER_OVF_VECT_ADDRESS " \n");
struct ThreadData
{
    volatile uint8_t status;
    void *(*t_Run)(void *);
    void *arg;
    uint16_t sp;
    uint8_t stack[STACK_SIZE];
};

static thread_t thread_id;
static struct ThreadData tarray[NUM_OF_THREAD];
static uint16_t main_st_pt;

ISR(THREAD_TIMER_OVF_VECT) // interrupt function of timer0 overflow vector
{
    asm(                // save all general register
        "PUSH R0 \n"    // Push register on stack
        "PUSH R1 \n"    // Push register on stack
        "PUSH R2 \n"    // Push register on stack
        "PUSH R3 \n"    // Push register on stack
        "PUSH R4 \n"    // Push register on stack
        "PUSH R5 \n"    // Push register on stack
        "PUSH R6 \n"    // Push register on stack
        "PUSH R7 \n"    // Push register on stack
        "PUSH R8 \n"    // Push register on stack
        "PUSH R9 \n"    // Push register on stack
        "PUSH R10 \n"   // Push register on stack
        "PUSH R11 \n"   // Push register on stack
        "PUSH R12 \n"   // Push register on stack
        "PUSH R13 \n"   // Push register on stack
        "PUSH R14 \n"   // Push register on stack
        "PUSH R15 \n"   // Push register on stack
        "PUSH R16 \n"   // Push register on stack
        "PUSH R17 \n"   // Push register on stack
        "PUSH R18 \n"   // Push register on stack
        "PUSH R19 \n"   // Push register on stack
        "PUSH R20 \n"   // Push register on stack
        "PUSH R21 \n"   // Push register on stack
        "PUSH R22 \n"   // Push register on stack
        "PUSH R23 \n"   // Push register on stack
        "PUSH R24 \n"   // Push register on stack
        "PUSH R25 \n"   // Push register on stack
        "PUSH R26 \n"   // Push register on stack
        "PUSH R27 \n"   // Push register on stack
    // #if (defined __AVR_ATmega2560__)
        "PUSH R28 \n"   // Push register on stack
        "PUSH R29 \n"
        "PUSH R30 \n"
        "PUSH R31 \n"
    // #endif
    ); // Push register on stack
    TCNT0 = 0;
    uint16_t st_pt = SP;
    if (thread_id == MIN_THREAD_ID)
    {
        main_st_pt = st_pt;
    }
    else
    {
        tarray[thread_id - MIN_THREAD_ID - 1].sp = st_pt;
    }

    thread_id = thread_id + 1;
    if (thread_id > (NUM_OF_THREAD + MIN_THREAD_ID))
    {
        thread_id = MIN_THREAD_ID;
    }
    if (thread_id != MIN_THREAD_ID)
    {
        while (tarray[thread_id - MIN_THREAD_ID - 1].status == 0)
        {
            thread_id = thread_id + 1;
            if (thread_id > (NUM_OF_THREAD + MIN_THREAD_ID))
            {
                thread_id = MIN_THREAD_ID;
                break;
            }
        }
    }

    if (thread_id == MIN_THREAD_ID)
    {
        st_pt = main_st_pt;
    }
    else
    {
        st_pt = tarray[thread_id - MIN_THREAD_ID - 1].sp;
    }

    SP = st_pt;
    asm( // restore all general register
    // #if (defined __AVR_ATmega2560__)
        "POP R31 \n"  // Pop register on stack
        "POP R30 \n"  // Pop register on stack
        "POP R29 \n"  // Pop register on stack
        "POP R28 \n"  // Pop register on stack
    // #endif
        "POP R27 \n"  // Pop register on stack
        "POP R26 \n"  // Pop register on stack
        "POP R25 \n"  // Pop register on stack
        "POP R24 \n"  // Pop register on stack
        "POP R23 \n"  // Pop register on stack
        "POP R22 \n"  // Pop register on stack
        "POP R21 \n"  // Pop register on stack
        "POP R20 \n"  // Pop register on stack
        "POP R19 \n"  // Pop register on stack
        "POP R18 \n"  // Pop register on stack
        "POP R17 \n"  // Pop register on stack
        "POP R16 \n"  // Pop register on stack
        "POP R15 \n"  // Pop register on stack
        "POP R14 \n"  // Pop register on stack
        "POP R13 \n"  // Pop register on stack
        "POP R12 \n"  // Pop register on stack
        "POP R11 \n"  // Pop register on stack
        "POP R10 \n"  // Pop register on stack
        "POP R9 \n"   // Pop register on stack
        "POP R8 \n"   // Pop register on stack
        "POP R7 \n"   // Pop register on stack
        "POP R6 \n"   // Pop register on stack
        "POP R5 \n"   // Pop register on stack
        "POP R4 \n"   // Pop register on stack
        "POP R3 \n"   // Pop register on stack
        "POP R2 \n"   // Pop register on stack
        "POP R1 \n"   // Pop register on stack
        "POP R0 \n"   // Pop register on stack
    );
    TCNT0 = 0;        // for 255 clock to flow
    TIMSK0 = (1 << TOIE0);
}

void Run()
{
    tarray[thread_id - MIN_THREAD_ID - 1].t_Run(tarray[thread_id - MIN_THREAD_ID - 1].arg); // call run() of thread
    thread_exit(NULL);
}

void init_mutil_thread()
{
    for (uint8_t i = 0; i < NUM_OF_THREAD; ++i)
    {
        tarray[i].status = 0; // disable
    }

    thread_id = MIN_THREAD_ID;

    /*
    setup timer 0  to use as trigger interrupt. it overflow at 255. prescaler for 64, it take 1,024 ms to overflow interrupt,
    */
    TCNT0 = 0; // for 256 clock to flow
    TCCR0A = 0x00;
    TCCR0B = (1 << CS01) | (1 << CS00); // Timer mode with 64 prescler
    TIMSK0 = (1 << TOIE0); // Enable timer0 overflow interrupt(TOIE0)

    /*
    setup timer 2 to use as timer. it overflow at 255. prescaler for 1024, that mean 0,064 ms per tick
    */
    TCNT2 = 0; // 255 clock to overflow
    TCCR2A = 0x00;
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); // Timer mode with 1024 prescaler
}

uint8_t thread_create(thread_t *t_id, void *(*func_Run)(void *), void *arg)
{
    uint8_t sts = ERROR;
    for (uint8_t i = 0; i < NUM_OF_THREAD; ++i)
    {
        if (tarray[i].status == 0)
        {
            tarray[i].t_Run = func_Run;
            tarray[i].arg = arg;
            memset(tarray[i].stack, 0, STACK_SIZE);
            tarray[i].stack[sizeof(tarray->stack) - 1] = (((uint16_t)&Run) & 0xFF);
            tarray[i].stack[sizeof(tarray->stack) - 2] = (((uint16_t)&Run >> 8) & 0xFF);
            tarray[i].sp = (uint16_t)&tarray[i].stack[sizeof(tarray->stack) - 52];
            tarray[i].status = 1; // run this thread
            *t_id = i + MIN_THREAD_ID + 1;
            sts = SUCCESS;
            break;
        }
    }

    return sts;
}

uint8_t thread_join(uint8_t t_id, void **retval)
{
    while (1)
    {
        if (tarray[t_id - MIN_THREAD_ID - 1].status == 0)
        {
            *retval = tarray[t_id - MIN_THREAD_ID - 1].arg;
            return SUCCESS;
        }
        THREAD_BLOCK();
    }
    return ERROR;
}

void thread_exit(void *retval)
{
    if (thread_id == MIN_THREAD_ID)
    {
        return; // thread main always run
    }
    tarray[thread_id - MIN_THREAD_ID - 1].arg = retval;
    tarray[thread_id - MIN_THREAD_ID - 1].status = 0; // stop
    THREAD_BLOCK();
}

thread_t thread_self()
{
    return thread_id;
}

void thread_sleep(uint16_t n)
{
    TIMSK0 = 0;
    uint8_t pre = TCNT2;
    uint8_t now = 0;
    uint16_t time = 0;
    while (1)
    {
        TIMSK0 = 0;
        now = TCNT2;
        if (now >= pre)
        {
            time += now - pre;
        }
        else
        {
            time += 255 + now - pre;
        }
        if (time > n)
        {
            break;
        }
        pre = now;
        THREAD_BLOCK();
    }
}

void thread_mutex_lock(volatile mt_lock_t *lock)
{
    if (*lock == thread_id)
        return;
    while (*lock != THREAD_UNLOCK)
    {
        THREAD_BLOCK();
    }
    THREAD_BLOCK();
    while (*lock != THREAD_UNLOCK)
    {
        THREAD_BLOCK();
    }
    *lock = thread_id;
    return;
}

void thread_mutex_unlock(volatile mt_lock_t *lock)
{
    if (*lock != thread_id)
        return;
    *lock = THREAD_UNLOCK;
    return;
}

uint8_t get_thread_stack_use_size(thread_t t_id)
{
    uint8_t size = 0;
    if (t_id == MIN_THREAD_ID)
    {
        if (thread_id == t_id)
        {
            size = RAMEND - SP;
        }
        else
        {
            size = RAMEND - main_st_pt;
        }
    }
    else
    {
        if (thread_id == t_id)
        {
            size = (uint16_t)&tarray[t_id - MIN_THREAD_ID - 1].stack[sizeof(tarray->stack) - 1] - SP;
        }
        else
        {
            size = (uint16_t)&tarray[t_id - MIN_THREAD_ID - 1].stack[sizeof(tarray->stack) - 1] - tarray[t_id - MIN_THREAD_ID - 1].sp;
        }
    }
    return size;
}

uint8_t get_thread_status(thread_t t_id)
{
    if ((t_id < MIN_THREAD_ID) || (t_id > (MIN_THREAD_ID + NUM_OF_THREAD)))
        return 0;
    if (t_id == MIN_THREAD_ID)
        return 1;
    return tarray[t_id - MIN_THREAD_ID - 1].status;
}
