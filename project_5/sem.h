#include "threads.h"

typedef struct Semaphore_s{
	int counter;
	queue_t *q;
}Semaphore_t;

Semaphore_t* CreateSem(int InputValue)
{
    Semaphore_t* sem = (Semaphore_t*)malloc(sizeof(Semaphore_t)); /* Allocating a semaphore structure */
    sem->counter = InputValue; /* Initializing the semaphore value to the InputValue */
    sem->q = newQueue(); /* Creating a new semaphore queue*/	

    return sem; /* returning the pointer to the semaphore */
}

void P(Semaphore_t* sem)
{
    TCB_t* Prev_Thread;
    sem->counter--; /* decrementing the semaphore count */
    if(sem->counter < 0)
    {
	/* blocking the thread in the queue associated with the semaphore */
	Prev_Thread = DelQueue(ReadyQ);
	AddQueue(sem->q,Prev_Thread);
	Curr_Thread = ReadyQ->head;	
	swapcontext(&Prev_Thread->context,&(Curr_Thread->context));
    }
}

void V(Semaphore_t* sem)
{
    sem->counter++; /* Incrementing the semaphore count */
    if(sem->counter <= 0)
    {
	/* Takeing a PCB out of the semaphore queue and puts it into the ReadyQ */
	AddQueue(ReadyQ, DelQueue(sem->q));
    }
    yeild();
}
