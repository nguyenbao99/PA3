#include "queue.h"
#include <stdlib.h>
#include <stdio.h>


int initQueue(queue* q, int size){
    // int charSize = sizeof(char*)*size;
    if(size > 0){
        q->size = size;
        q->c = malloc(sizeof(char*)*size);
        if(!(q->c)){
            fprintf(stderr, "Error when trying to create queue\n");
            return 1;
        }
        for(int i = 0; i < size; i++){
            q->c[i] = '\0';
        }
        q->front = 0;
        q->rear = 0;
        q->numOfElements = 0;
        return q->size;
    }
    else{
        fprintf(stderr, "Error: invalid size\n");
    }
    return 1;
}

int emptyQueue(queue* q){
    if((q->front == q->rear) && (q->c[q->front] == '\0')){
        return 1;
    }
    else{
         return 0;
    }
}

int fullQueue(queue* q){
    if((q->front == q->rear) && (q->c[q->front] != '\0')){
        return 1;
    }
    else{
        return 0;
    }
    
}

int pushQueue(queue* q, char* addToQueue){
    if(fullQueue(q)){
        fprintf(stderr, "Error: Queue is already full \n");
        return 1;
    }
    q->c[q->rear] = addToQueue;
    q->rear = ((q->rear+1) % (q->size));
    q->numOfElements++;
    return 0;
}

char* popQueue(queue* q){
    if(emptyQueue(q)){
        // fprintf(stderr, "Error: Queue is already empty \n");
        return '\0';
    }
    char* popped = q->c[q->front];
    q->c[q->front]= '\0';
    q->front = ((q->front+1) % (q->size));
    q->numOfElements--;
    return popped;
}

int queueSize(queue* q){
    return q->numOfElements;
}

void freeQueue(queue* q){
    while(!emptyQueue(q)){
        popQueue(q);
    }
    free(q->c);
}
