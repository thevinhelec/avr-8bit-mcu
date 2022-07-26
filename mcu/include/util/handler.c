/*
 * thread.c
 *
 * Created: 24/7/2022 10:56:42 AM
 *  Author: thevinh
 */

#include "util/handler.h"
#include "stddef.h"

void *handler_loop(void *arg)
{
    while (1)
    {
        struct Handler *handler = (struct Handler *)arg;
        //thread_mutex_lock(handler->mt_lock);
        if (isEmpty(handler->message_queue) == 1)
        {
            //thread_mutex_unlock(handler->mt_lock);
            thread_sleep(100);
        }
        else
        {
            void *message_pointer = dequeue(handler->message_queue);
            //thread_mutex_unlock(handler->mt_lock);
            handler->func_handler(message_pointer);
        }
    }
    return NULL;
}

uint8_t handler_create(struct Handler *handler, void (*func)(void *), struct Cqueue *queue)
{
    thread_t t_id;
    uint8_t status = thread_create(&t_id, handler_loop, handler);
    if (status != SUCCESS)
        return status;

    handler->thread_hander_id = t_id;
    handler->func_handler = func;
    handler->message_queue = queue;
    handler->mt_lock = THREAD_UNLOCK;
    return SUCCESS;
}

uint8_t addMessage(struct Handler *handler, void *message)
{
    //thread_mutex_lock(handler->mt_lock);
    if (isFull(handler->message_queue) == 1)
    {
        //thread_mutex_unlock(handler->mt_lock);
        return ERROR;
    }
    enqueue(handler->message_queue, message);
    //thread_mutex_unlock(handler->mt_lock);
    return SUCCESS;
}