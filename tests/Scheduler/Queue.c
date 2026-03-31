#include "Queue.h"

static int _enqueue(SchedulerQueue *queue, uint8_t* item);
static uint8_t* _dequeue(SchedulerQueue *queue);
static void _init_queue(SchedulerQueue *queue);

// OPTIONAL
static void _init_queue(SchedulerQueue *queue) {
    queue->start = 0;
    queue->end = 0;
    queue->init = &_init_queue;
    queue->enqueue = &_enqueue;
    queue->dequeue = &_dequeue;
}

static int isEmpty(SchedulerQueue *q)
{
    return (q->start == q->end);
}

static int isFull(SchedulerQueue *q)
{
    return ((q->end + 1) % MAX_SIZE == q->start);
}

static void increase_index(int *index) {
    if (*index == MAX_SIZE - 1) {
        *index = 0;
        return;
    }
    (*index)++;
}

static int _enqueue(SchedulerQueue *queue, uint8_t* item) {
    if (queue == NULL)
        return -1;
    if (isFull(queue))
        return -1;

    memcpy(queue->items[queue->end], item, 128);
    increase_index(&queue->end);
    return 0;
}

static uint8_t* _dequeue(SchedulerQueue *queue) {
    if (queue == NULL || isEmpty(queue))
        return NULL;

    uint8_t *result = queue->items[queue->start];
    increase_index(&queue->start);

    return result;
}

SchedulerQueue SchedulerQueue_default = {
    .start = 0,
    .end = 0,
    .init = &_init_queue,
    .enqueue = &_enqueue,
    .dequeue = &_dequeue
};