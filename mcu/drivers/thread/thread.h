/*
 * mutil_thread.h
 *
 * Created: 12/11/2021 10:40:10 AM
 *  Author: thevinh
 */

/************************************************************************/
/* support multi thread on atemga328. osc frezquence is 16Mhz
It use timer0 and timer2 to make scheduler. Slot time = 0.128 ms.
It can running MAX_THREAD + 1 thread at time.
Every thread has it own stack, size = 200 byte. Except main thread, its
stack size is remain memory.
Main thread has id = 0. Other stack id is will reside from 1 to MAX_THREAD

example code:
static uint8_t lock = MT_UNLOCK;
void RunT1(){
	mt_lock(&lock);
	//do some thing
	mt_unlock(&lock);
}
int main(void)
{	
	thread_init_mutil();
	thread_create(RunT1)); // create and start
	sei(); // enable globle interrupt
	while(1){
		
	}
}                                                                     */
/************************************************************************/
#ifndef THREAD_H_
#define THREAD_H_

#include <stdint.h>
#include <avr/interrupt.h>

#define MAX_THREAD 4
#define STACK_SIZE 250
#define THREAD_UNLOCK 255
#define ERROR 255
#define SUCCESS 0
#define MIN_THREAD_ID 10

typedef uint8_t thread_t;            /* identify a thread */
typedef uint8_t mt_lock_t;            /* identify a thread */
/* initial environments to run multi thread
*/
void init_mutil_thread();

/* create a new execute routine specify by function p_Run.
parameter p_Run : a function to execute

return uint_8 : return 0 if success. other as error
*/
uint8_t thread_create(thread_t *t_id, void *(*func_Run)(void *), void *arg);

/* Block the calling thread until thread t_id stop. 
parameter t_id : id of thread
return uint_8 : return 0 if success. other as error
*/
uint8_t thread_join(thread_t t_id, void **retval);

/* Forces the calling thread to stop executing. 
parameter retval : return value to thread.join()
return void : no return
*/
void thread_exit(uint8_t *retval);

/* return id of calling thread
return uint_8 : id of current thread
*/
thread_t thread_self();

/*causes the current thread to suspend execution for a specified period,
release time slot to other thread. these sleep times are not guaranteed 
to be precise. 
	sleep time = n * 0.064 ms  (at 16MHz)
parameter n :
return void
*/
void thread_sleep(uint16_t n);

/* Locks a object, which identifies a lock. If the lock is already
locked by another thread, the thread waits for the lock to become available
parameter *lock : pointer to the lock
return void
*/
void thread_mutex_lock(mt_lock_t  *lock);

/* unLocks a object, which identifies a lock.
parameter *lock : pointer to the lock
return void
*/
void thread_mutex_unlock(mt_lock_t *lock);

/* get stack size of a thread
parameter t_id : id of thread
return int : size of stack
*/
uint8_t get_stack_size(thread_t t_id);

/* get status of a thread
parameter t_id : id of thread
return int : status of stack
*/
uint8_t get_status(thread_t t_id);

#endif /* THREAD_H_ */