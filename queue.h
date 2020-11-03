#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct queue{
    char* *c;
    int numOfElements;
    int front;
    int rear;
    int size;
}queue;

int initQueue(queue* q, int size);
int emptyQueue(queue* q);
int fullQueue(queue* q);
int pushQueue(queue* q, char* addToQueue);
char* popQueue(queue* q);
int queueSize(queue* q);
void freeQueue(queue* q);

#endif