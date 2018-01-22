#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define PRODUCER 0
#define CONSUMER 1
#define BUFFER_SIZE 50
#define ROUNDS 3

#define DEBUG

// type definition
typedef struct _thread_data_t {
    int tid;
    double stuff;
} thread_data_t;

// function prototypes
void* produce(void*);
void* consume(void*);

// global variables
int stack[BUFFER_SIZE];
int counter = 0, rear = 0, front = 0, i;
bool flag[2];

// methods
int main() 
{
    pthread_t tPr, tCo;
    int pid = PRODUCER, cid = CONSUMER, rc;
    thread_data_t tPr_data, tCo_data;
    tPr_data.tid = pid;
    tCo_data.tid = cid;
    
    for (i = 0; i < ROUNDS; ++i)
    {
        if ((rc = pthread_create(&tCo, NULL, consume, &tCo_data)))
        {
            fprintf(stderr, "error: consumer thread creation, rc: %d\n", rc);
            return EXIT_FAILURE;
        } 
        else
            printf("Consumer thread created with ID: %d\n", tCo_data.tid);
   
        if ((rc = pthread_create(&tPr, NULL, produce, &tPr_data)))
        {
            fprintf(stderr, "error: producer thread creation, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
        else
            printf("Producer thread created with ID: %d\n", tPr_data.tid);
   
        pthread_join(tPr, NULL);
        pthread_join(tCo, NULL);
    }    
    return EXIT_SUCCESS;
}

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
            flag[PRODUCER] = false;
        }
    } while(counter < BUFFER_SIZE - 1);

    return EXIT_SUCCESS; 
}

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
            flag[CONSUMER] = false;
        }
    } while(counter > 0); 
    return EXIT_SUCCESS;
}
