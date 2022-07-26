/*
 * thread.c
 *
 * Created: 12/11/2021 10:56:42 AM
 *  Author: thevinh
 */

#include "thread.h"
#include "UARTService.h"
#include "stddef.h"
#include <string.h>
struct ThreadData
{
    uint8_t status;
    void *(*t_Run)(void *);
    void *arg;
    uint16_t sp;
    uint8_t stack[STACK_SIZE];
};

static thread_t thread_id;
static struct ThreadData tarray[MAX_THREAD];
static uint16_t main_st_pt;

ISR(TIMER0_OVF_vect) // interrupt function of timer0 overflow vector
{
    asm(                // save all general register
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
        "PUSH R22 \n"   // Push register on stack
        "PUSH R23 \n"   // Push register on stack
        "PUSH R26 \n"   // Push register on stack
        "PUSH R27 \n");   // Push register on stack
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
    if (thread_id > (MAX_THREAD + MIN_THREAD_ID))
    {
        thread_id = MIN_THREAD_ID;
    }
    if (thread_id != MIN_THREAD_ID)
    {
        while (tarray[thread_id - MIN_THREAD_ID - 1].status == 0)
        {
            thread_id = thread_id + 1;
            if (thread_id > (MAX_THREAD + MIN_THREAD_ID))
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
    asm(              // restore all general register
        "POP R27 \n"  // Pop register on stack
        "POP R26 \n"  // Pop register on stack
        "POP R23 \n"  // Pop register on stack
        "POP R22 \n"  // Pop register on stack
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
        "POP R2 \n"); // Pop register on stack
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
    for (uint8_t i = 0; i < MAX_THREAD; ++i)
    {
        tarray[i].status = 0; // disable
    }

    thread_id = MIN_THREAD_ID;

    /*
    setup timer 0  to use as trigger interrupt. it over follow at 255. prescaler for 8, it take 0.128 ms to overflow interrupt,
    */
    TCNT0 = 0; // for 256 clock to flow
    TCCR0A = 0x00;
    TCCR0B = (1 << CS01);  //| (1<<CS00);;  // Timer mode with 8 prescler
    TIMSK0 = (1 << TOIE0); // Enable timer0 overflow interrupt(TOIE0)

    /*
    setup timer 2 to use as timer. it over follow at 255. prescaler for 1024, that mean 0.064 ms per tick
    */
    TCNT2 = 0; // 255 clock to flow
    TCCR2A = 0x00;
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20); // Timer mode with 1024 prescaler
}

uint8_t thread_create(thread_t *t_id, void *(*func_Run)(void *), void *arg)
{
    uint8_t sts = ERROR;
    for (uint8_t i = 0; i < MAX_THREAD; ++i)
    {
        if (tarray[i].status == 0)
        {
            tarray[i].t_Run = func_Run;
            tarray[i].arg = arg;
            memset(tarray[i].stack, 0, STACK_SIZE);
            tarray[i].stack[sizeof(tarray->stack) - 1] = (((uint16_t)&Run) & 0xFF);
            tarray[i].stack[sizeof(tarray->stack) - 2] = (((uint16_t)&Run >> 8) & 0xFF);
            tarray[i].sp = (uint16_t)&tarray[i].stack[sizeof(tarray->stack) - 38];
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
    TIMSK0 = 0; // disable over flow interrupt of timer0
    while (1)
    {
        TIMSK0 = 0;
        if (tarray[t_id - MIN_THREAD_ID - 1].status == 0)
        {
            *retval = tarray[t_id - MIN_THREAD_ID - 1].arg;
            return SUCCESS;
        }
        asm(
            "CLI \n"               // disable global interrupt
            "CALL 0x00000040 \n"); // call over flow interrupt vector of timer0 directly
    }
    return ERROR;
}

void thread_exit(thread_t *retval)
{
    if (thread_self() == MIN_THREAD_ID)
    {
        return; // thread main always run
    }
    TIMSK0 = 0;
    tarray[thread_self() - MIN_THREAD_ID - 1].arg = retval;
    tarray[thread_self() - MIN_THREAD_ID - 1].status = 0; // stop
    asm(
        "CLI \n"               // disable global interrupt
        "CALL 0x00000040 \n"); // call over flow interrupt vector of timer0 directly
}

thread_t thread_self()
{
    return thread_id;
}

void thread_sleep(uint16_t n)
{
    TIMSK0 = 0; // disable over flow interrupt of timer0
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
        asm(
            "CLI \n"               // disable global interrupt
            "CALL 0x00000040 \n"); // call over flow interrupt vector of timer0 directly
    }
}

void thread_blocked()
{
    TIMSK0 = 0;
    asm(
        "CLI \n"               // disable global interrupt
        "CALL 0x00000040 \n"); // call over flow interrupt vector of timer0 directly
}

void thread_mutex_lock(uint8_t *lock)
{
    //if (*lock == thread_self()) return;
    while (*lock != THREAD_UNLOCK)
    {
        thread_blocked();
    }
    thread_blocked();
    while (*lock != THREAD_UNLOCK)
    {
        thread_blocked();
    }
    if (*lock == THREAD_UNLOCK)
    {
        *lock = thread_self();
    }
    return;
}

void thread_mutex_unlock(uint8_t *lock)
{
    // if (*lock != thread_self())
    // {
    //     return;
    // }
    *lock = THREAD_UNLOCK;
    return;
}

uint8_t get_stack_size(thread_t t_id)
{
    uint8_t size = 0;
    if (t_id == MIN_THREAD_ID)
    {
        if (thread_self() == t_id)
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
        if (thread_self() == t_id)
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

uint8_t get_status(thread_t t_id)
{
    return tarray[t_id - MIN_THREAD_ID - 1].status;
}
