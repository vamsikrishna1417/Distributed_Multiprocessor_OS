#include "q.h"

#define STACK_SIZE 8192

queue_t* ReadyQ=NULL;
TCB_t* Curr_Thread = NULL;
int counter=0;

void print_threadId(TCB_t* tcb)
{
    printf("Current Thread_id:%d\n",tcb->thread_id);
}

void start_thread(void (*function)(void))
{
    void *stack = malloc(STACK_SIZE);
    TCB_t* tcb = NewItem();
    /* Creating a new queue, if ready queue is empty */
    if(!ReadyQ) ReadyQ = newQueue();
    init_TCB(tcb,function,stack,STACK_SIZE);
    tcb->thread_id = ++counter;
    AddQueue(ReadyQ,tcb);
}

void run()
{
    Curr_Thread = ReadyQ->head;
    /* get a place to store the main context, for faking */
    ucontext_t parent;
    /* magic sauce */
    getcontext(&parent);
    if(Curr_Thread)
	swapcontext(&parent,&(Curr_Thread->context));
}

void yeild()
{
    TCB_t* Prev_Thread;
    AddQueue(ReadyQ, DelQueue(ReadyQ));
    Prev_Thread = Curr_Thread;
    Curr_Thread = ReadyQ->head;
    if(Curr_Thread){
	swapcontext(&(Prev_Thread->context),&(Curr_Thread->context));
    }
}

