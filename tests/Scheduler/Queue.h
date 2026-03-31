#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_SIZE 100

typedef struct {
    uint32_t t;
    uint8_t *cmd;
} queueItem;

typedef struct SchedulerQueue SchedulerQueue;

struct SchedulerQueue{
    int start;
    int end;
    uint8_t items[MAX_SIZE][128];

    void (*init)(SchedulerQueue *queue);
    int (*enqueue)(SchedulerQueue *queue, uint8_t *item);
    uint8_t* (*dequeue)(SchedulerQueue *queue);
};

extern SchedulerQueue SchedulerQueue_default;

#endif