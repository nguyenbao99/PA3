#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h> // To use pid_t
#include <sys/stat.h> // To use gettid
#include <sys/types.h> // To use gettid
#include "multi-lookup.h"

#define GNU_SOURCE

void* requester(void *id){
    char* domain;
    char name[1025];
    char* fileName;
    int reset = 0;
    int numOfServiced = 0; // this is for number of files serviced
    pid_t tid;
    tid = pthread_self(); // to get the thread id

    struct thread* thread = id;

    if(thread->numOfThreads > thread->numOfFiles){
        pthread_mutex_lock(thread->mutexServiced); //lock current file when accessing
        fprintf(thread->tFile, "Thread %d serviced %d files \n", tid, numOfServiced);
        pthread_mutex_unlock(thread->mutexServiced);
        return NULL;
    }

    while(!emptyQueue(thread->qFile)){
        pthread_mutex_lock(thread->inputMutex);//lock so mult threads can't request same file
        if(!emptyQueue(thread->qFile)){
            fileName = popQueue(thread->qFile);
        }
        pthread_mutex_unlock(thread->inputMutex);
        FILE* inputFile = fopen(fileName, "r"); 
        while(fscanf(inputFile, "%s", name) > 0){
            while(reset != 1){
                domain = malloc(1025);
                strncpy(domain, name, 1025);
                pthread_mutex_lock(thread->bufferMutex);
                while(fullQueue(thread->buffer)){ // loops until there's space
                    pthread_mutex_unlock(thread->bufferMutex);
                    usleep(rand()%100); //stops the execution for n ms
                    pthread_mutex_lock(thread->bufferMutex);
                }
                pushQueue(thread->buffer, domain); //push onto buffer queue
                //printf("%s",domain);
                pthread_mutex_unlock(thread->bufferMutex);
                reset = 1; //to exit loop
            }
            reset = 0; //to continue into next loop
        }
        fclose(inputFile);
        numOfServiced++; 
    }
    
    pthread_mutex_lock(thread->mutexServiced);
    fprintf(thread->tFile,"Thread %d serviced %d files \n", tid, numOfServiced);
    pthread_mutex_unlock(thread->mutexServiced);
    return NULL;

}

void* resolver(void *id){
    struct thread* thread = id;
    char ipAddr[INET6_ADDRSTRLEN];
    char* popDomain;
    
    while(!emptyQueue(thread->buffer) || emptyQueue(thread->stillReq) == 0){
        pthread_mutex_lock(thread->bufferMutex);
        popDomain = popQueue(thread->buffer); // this is to access the domain name
        if(popDomain == NULL){
            pthread_mutex_unlock(thread->bufferMutex);
            usleep(rand()%100);
        }
        else{
                pthread_mutex_unlock(thread->bufferMutex);
                if(dnslookup(popDomain,ipAddr, sizeof(ipAddr)) == UTIL_FAILURE){
                    strncpy(ipAddr,"",sizeof(ipAddr));
                    fprintf(stderr,"DNS ERROR: %s \n", popDomain);
                }
                pthread_mutex_lock(thread->outputMutex);
                fprintf(thread->tFile,"%s,%s,\n", popDomain,ipAddr);
                pthread_mutex_unlock(thread->outputMutex);
            }
            free(popDomain);
    }
    return NULL;
}

int main(int argc, char* argv[]){
    pthread_mutex_t bufferMutex;
    pthread_mutex_t mutexServiced;
    pthread_mutex_t inputMutex;
    pthread_mutex_t outputMutex;
    
    queue requestsLeft; 
	queue buffered; 
	queue queueFile; 

    char *temp1 = argv[1];
	char *temp2 = argv[2];
	int numOfRequests = atoi(temp1); //string to int
	int numOfResolves = atoi(temp2);

	int numInputFiles = argc-5 ;

	pthread_t requests[numOfRequests]; // array for requester threads
	pthread_t resolves[numOfResolves]; // array for resolver threads
    if(argc < 6){
        printf("Error: Number of Arguments doesn't match");
        return 0;
    }
    FILE* outputFile = fopen(argv[4], "w");
    if(!outputFile){
        fprintf(stderr, "Error: Output file is invalid");
        return 0;
    }

    FILE* fileServiced = fopen(argv[3], "w");
    if(!fileServiced){
        fprintf(stderr, "Error: File is invalid");
    }

	// Initialized mutexes to protect shared resources
	pthread_mutex_init(&bufferMutex, NULL);
	pthread_mutex_init(&outputMutex, NULL);
	pthread_mutex_init(&inputMutex, NULL);
	pthread_mutex_init(&mutexServiced, NULL);

    initQueue(&requestsLeft,1); // Queue for existing requests
	initQueue(&buffered, 10); // Shared buffer for requester and resolver threads
	initQueue(&queueFile, numInputFiles); // Files to be parsed

	// Record of existing requestor threads
	pushQueue(&requestsLeft, "There are still requester threads left");


    int index = 5;
	char* file;
	for(int i = 0; i < numInputFiles; i++){
		file = argv[index];
		pushQueue(&queueFile, file);
		index++;
	}

	// Requester threads information
	struct thread getReqInfo[numOfRequests];
	// Resolver threads information
	struct thread getResInfo[numOfResolves];

	//Begin tracking performance
	struct timeval start, end;
	gettimeofday(&start, NULL);

	// Create requester threads and store their information
	for (int i = 0; i < numOfRequests; i++){
		getReqInfo[i].numOfThreads = i+1;
		getReqInfo[i].numOfFiles = numInputFiles;
		getReqInfo[i].qFile = &queueFile;
		getReqInfo[i].buffer = &buffered;
		getReqInfo[i].bufferMutex = &bufferMutex;
		getReqInfo[i].inputMutex = &inputMutex;
		getReqInfo[i].mutexServiced = &mutexServiced;
		getReqInfo[i].tFile = fileServiced;
		pthread_create(&(requests[i]),NULL, requester, &(getReqInfo[i]));
	}
	
	// Create resolver threads and store their information
	for (int i = 0; i < numOfResolves; i++){
		getResInfo[i].tFile = outputFile;
		getResInfo[i].buffer = &buffered;
		getResInfo[i].bufferMutex = &bufferMutex;
		getResInfo[i].outputMutex = &outputMutex;
		getResInfo[i].stillReq = &requestsLeft;
		pthread_create(&(resolves[i]), NULL, resolver, &(getResInfo[i]));
	}

	// Join requestor threads
	for (int i = 0; i < numOfRequests; i++){
		pthread_join(requests[i], NULL);
	}

	popQueue(&requestsLeft); //All requestor threads have finished

	// Join resolver threads
	for (int i = 0; i < numOfResolves; i++){
		pthread_join(resolves[i], NULL);
	}

	//Close Files
	fclose(outputFile);
	fclose(fileServiced);

	// Destroy mutexes	
	pthread_mutex_destroy(&bufferMutex);
	pthread_mutex_destroy(&outputMutex);
	pthread_mutex_destroy(&inputMutex);
	pthread_mutex_destroy(&mutexServiced);

	// Free memory
	freeQueue(&requestsLeft);
	freeQueue(&queueFile);
	freeQueue(&buffered);

	// End tracking performance
	gettimeofday(&end, NULL);

	// Print information for performance.txt
	printf("Number for requestor thread = %d\n", numOfRequests);
	printf("Number for resolver thread = %d\n", numOfResolves);
	printf("Total run time: %ld\n", ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec));
	return 0;

}