#include <cstdlib>
#include <iostream>
#include <sysrepo.h>
#include <pthread.h>
#include <unistd.h>
#include "getData.hpp"
#include <stdlib.h>

#define NUM_THREADS 1
#define DEFAULT_STACK_SIZE 0x10000

// wrapper function that calls fullLifecycleDataGet
void* wrapper(void* params) {
    fullLifecycleDataGet();

    pthread_exit(NULL);
    return NULL;
};


int main(int argc, char** argv) {
    int defaultStackSize = DEFAULT_STACK_SIZE;
    int parsedValue;
    int opt;
    while ((opt = getopt(argc, argv, "s:")) != -1) 
    {
        switch (opt) 
        {
        case 's':
            parsedValue = strtoul(optarg, NULL, 0) ;
            
            if (parsedValue > DEFAULT_STACK_SIZE) {
                printf("Setting default stack size for each thread to %d\n", parsedValue);
                defaultStackSize = parsedValue;
            } else {
                printf("stack size %d must be greator than %d\n", parsedValue, DEFAULT_STACK_SIZE);
                defaultStackSize = DEFAULT_STACK_SIZE;
            }

            break;
        default: 
            defaultStackSize = DEFAULT_STACK_SIZE;
        }
    }

    // Call in main thread
    fullLifecycleDataGet();

    // Create threads
    pthread_t thread[NUM_THREADS];
    int rc;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, defaultStackSize);
        rc = pthread_create(&thread[i], &attr, wrapper, NULL);
        pthread_attr_destroy(&attr);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread[i], NULL);
    }

    pthread_exit(NULL);

    return 0;
}