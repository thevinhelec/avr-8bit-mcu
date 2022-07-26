/*
 * circular_queue.c
 *
 * Created: 12/13/2021 11:16:02 PM
 *  Author: thevi
 */

#include "util/circular_queue.h"
#include <string.h>
void setupCqueue(struct Cqueue *c_queue, uint16_t *static_array, uint8_t capacity)
{
    c_queue->capacity = capacity;
    c_queue->front = 0;
    c_queue->size = 0;

    // This is important, see the enqueue
    c_queue->rear = capacity - 1;
    memset((void *)static_array, 0, sizeof(uint16_t) * capacity);
    c_queue->array = static_array;
}

uint8_t isFull(struct Cqueue *c_queue)
{
    return (c_queue->size == c_queue->capacity);
}

uint8_t isEmpty(struct Cqueue *c_queue)
{
    return (c_queue->size == 0);
}

void enqueue(struct Cqueue *c_queue, uint16_t item)
{
    c_queue->rear = (c_queue->rear + 1) % c_queue->capacity;
    c_queue->array[c_queue->rear] = item;
    c_queue->size = c_queue->size + 1;
}

uint16_t dequeue(struct Cqueue *c_queue)
{
    uint16_t item = c_queue->array[c_queue->front];
    c_queue->front = (c_queue->front + 1) % c_queue->capacity;
    c_queue->size = c_queue->size - 1;
    return item;
}

uint8_t get_size(struct Cqueue *c_queue)
{
    return c_queue->size;
}

uint8_t get_free_size(struct Cqueue *c_queue)
{
    return c_queue->capacity - c_queue->size;
}

uint16_t front(struct Cqueue *c_queue)
{
    return c_queue->array[c_queue->front];
}

uint16_t rear(struct Cqueue *c_queue)
{
    return c_queue->array[c_queue->rear];
}