// #ifndef MULTI-LOOKUP_H
// #define MULTI-LOOKUP_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include "util.h"
#include "queue.h"
void* request(void *id);
void* resolve(void *id);

struct thread{
    int numOfThreads;
    int numOfFiles;
    FILE* tFile;
    queue* buffer;
    queue* qFile;
    queue* stillReq;
    pthread_mutex_t* bufferMutex;
    pthread_mutex_t* mutexServiced;
    pthread_mutex_t* inputMutex;
    pthread_mutex_t* outputMutex;
};
// #endif



