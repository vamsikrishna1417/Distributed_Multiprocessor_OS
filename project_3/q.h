#include "TCB.h"

typedef struct queue_s{
	TCB_t* head;
}queue_t;

/* Return a pointer to new item, which needs to be added the queue */
TCB_t* NewItem()
{
    return (TCB_t*)malloc(sizeof(TCB_t));	
}

queue_t* newQueue()
{
    queue_t *q = (queue_t*)malloc(sizeof(queue_t));
    q->head = NULL;
    return q;
}

/* Adds a queue item to the ready queue */
void AddQueue(queue_t *q, TCB_t* item)
{
    /* If head is not null, add the item at the end of the queue */
    if(item)
    {
	if(q->head){
	    TCB_t* tail = q->head->prev;
	    tail->next = item;
	    item->prev = tail;
	    item->next = q->head;
	    q->head->prev = item;
	}
	/* If head is null, point head to item */
	else{
	    q->head=item;
	    q->head->prev=q->head;
	    q->head->next=q->head;
	}
    }
}

/* deletes an item from the ready queue */
TCB_t* DelQueue(queue_t *q)
{
    /* If head is not null, delete the item in front */
    if(q->head == q->head->prev)
    {
	TCB_t* item = q->head;
	q->head = NULL;
	return item;
    }
    if(q->head){
	TCB_t* item = q->head;
	q->head = q->head->next;
	q->head->prev = item->prev;
	item->prev->next = q->head;
	item->next = NULL;
	item->prev = NULL;
	return item;
    }
    /* If head is null, return null */
    else return NULL;
}
