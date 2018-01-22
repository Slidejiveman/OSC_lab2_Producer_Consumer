#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define PRODUCER 0
#define CONSUMER 1
#define BUFFER_SIZE 10
#define ROUNDS 3

// toggle comments on DEBUG to see differing behaviors
// of this code based on sleep values.
// #define DEBUG

// type definition for thread specific data
// this is not needed in this example, but
// included for practice.
typedef struct _thread_data_t {
    int tid;
    double stuff;
} thread_data_t;

// function prototypes: alert compiler of
// their existence
void* produce(void*);
void* consume(void*);

// global variables: shared memory between producer
// and consumer
int stack[BUFFER_SIZE];
int counter = 0, rear = 0, front = 0, i;
bool flag[2];

// methods: Code Section
// Main establishes the threads. It is designed so that the producer
// fills the stack before allowing the consumer to consume them all.
// This is, essentially, batch processing.
int main() 
{
    pthread_t tPr, tCo;                      // thread identifier
    int pid = PRODUCER, cid = CONSUMER, rc;  // IDs, return code
    thread_data_t tPr_data, tCo_data;        // thread data
    tPr_data.tid = pid;                      // store id
    tCo_data.tid = cid;                      // "      "
    
    // ROUNDS controls the number of production and consumption cycles
    for (i = 0; i < ROUNDS; ++i)
    {
        // create the consumer thread with error handling
        // the consumer thread is created first so. It will
        // be kind and allow the producer to proceed before it.
        if ((rc = pthread_create(&tCo, NULL, consume, &tCo_data)))
        {
            fprintf(stderr, "error: consumer thread creation, rc: %d\n", rc);
            return EXIT_FAILURE;
        } 
        else
           #ifdef DEBUG
            printf("Consumer thread created with ID: %d\n", tCo_data.tid);
           #endif        

        // create the producer thread with error handling
        if ((rc = pthread_create(&tPr, NULL, produce, &tPr_data)))
        {
            fprintf(stderr, "error: producer thread creation, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
        else
           #ifdef DEBUG
            printf("Producer thread created with ID: %d\n", tPr_data.tid);
           #endif

        // block for thread completion before exiting
        pthread_join(tPr, NULL);
        pthread_join(tCo, NULL);
    }    
    return EXIT_SUCCESS;
}

// This function produces an integer value in the global
// stack buffer that can later be read by a consumer.
void *produce(void *arg)
{

    int turn;

    do { 
       
        flag[PRODUCER] = true;
        turn           = CONSUMER;
   
        while (flag[CONSUMER] && turn == CONSUMER) 
        {
          ; // go to sleep
        }
        if (counter < BUFFER_SIZE - 1)
        {
            stack[rear] = counter; // produces an int
            printf("Produced int with counter: = %d\n", counter);
            rear = (rear + 1) % BUFFER_SIZE;
            counter++;
        }
        flag[PRODUCER] = false;
        #ifdef DEBUG
         sleep(1);
        #endif
    } while(counter < BUFFER_SIZE - 1);
    #ifdef DEBUG
     printf("Successfully exiting produce function...\n");
    #endif
    return EXIT_SUCCESS; 
}

// This function consumes a value from the global stack
// AFTER it has been produced by the producer.
void *consume(void *arg)
{
    int turn;
    int consumed = 0;

    do {
        flag[CONSUMER] = true;
        turn           = PRODUCER;
        while(flag[PRODUCER] && turn == PRODUCER)
        {
         ;// go to sleep
        }
        if (counter > 0)
        {
            consumed = stack[front];
            printf("Consumed int: = %d\n", consumed);
            front = (front + 1) % BUFFER_SIZE;
            counter--;        
        }
        flag[CONSUMER] = false;
        #ifdef DEBUG
         sleep(1);
        #endif
    } while(counter > 0); 
    #ifdef DEBUG
     printf("Successfully exiting consume function...\n");
    #endif
     return EXIT_SUCCESS;
}
