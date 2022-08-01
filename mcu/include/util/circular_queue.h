/*
 * circular_queue.h
 *
 *  Author: thevinh
 */

#ifndef CIRCULAR_QUEUE_H_
#define CIRCULAR_QUEUE_H_

#include <stdint.h>

struct Cqueue
{
    uint8_t front;
    uint8_t rear;
    uint8_t size;
    uint8_t capacity;
    uint16_t *array;
};

void setupCqueue(struct Cqueue *c_queue, uint16_t *array, uint8_t capacity);

/*
Check if the queue is full
return uint8_t : 1 if true, 0 if false
*/
uint8_t isFull(struct Cqueue *c_queue);

/*
Check if the queue is empty
return uint8_t : 1 if true, 0 if false
*/
uint8_t isEmpty(struct Cqueue *c_queue);

/*
add an item to the c_queue (at rear). need to check isFull before enqueue
parameter *c_queue : poiter to struct Cqueue
          item : item
return void
*/
void enqueue(struct Cqueue *c_queue, uint16_t item);

/*
get an item (at front) from c_queue. remove item from queue. need to check isEmpty before desqueue
parameter *c_queue : poiter to struct Cqueue
return uint_8 : item
*/
uint16_t dequeue(struct Cqueue *c_queue);

/*
get number of item in c_queue.
parameter *c_queue : poiter to struct Cqueue
return uint_8 : number of item
*/
uint8_t getSize(struct Cqueue *c_queue);

/*
get number of remain slot in c_queue.
parameter *c_queue : poiter to struct Cqueue
return uint_8 : number of free slot
*/
uint8_t getFreeSize(struct Cqueue *c_queue);

/*
see item at front of c_queue. not remove item from queue
parameter *c_queue : poiter to struct Cqueue
return uint_8 : front
*/
uint16_t front(struct Cqueue *c_queue);

/*
see item at rear of c_queue. not remove item from queue
parameter *c_queue : poiter to struct Cqueue
return uint_8 : front
*/
uint16_t rear(struct Cqueue *c_queue);

#endif /* CIRCULAR_QUEUE_H_ */