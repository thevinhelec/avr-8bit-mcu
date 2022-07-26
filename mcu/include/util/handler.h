/*
 * thread.c
 *
 * Created: 24/7/2022 10:56:42 AM
 *  Author: thevinh
 */

#ifndef HANDLER_H_
#define HANDLER_H_

#include "thread.h"
#include "util/circular_queue.h"
struct Handler
{
    thread_t thread_hander_id;
    void (*func_handler)(void *);
    struct Cqueue *message_queue;
    volatile mt_lock_t mt_lock;
};

/* create a new Handler.
parameter   func : a function to handle message
            queue : message queue
return uint_8 : return 0 if success. other as error
*/
uint8_t handler_create(struct Handler *handler, void (*func)(void *), struct Cqueue *queue);


/* add a massage to Handler.
parameter   handler : 
            message : message to add
return uint_8 : return 0 if success. other as error
*/
uint8_t addMessage(struct Handler *handler, void * message);


#endif /* HANDLER_H_ */