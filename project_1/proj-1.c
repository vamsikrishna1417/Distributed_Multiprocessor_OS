/* PRUDHVI GALI - 1213366386
   VAMSI KRISHNA LANKA - 1215371417 
   */

#include <stdio.h>
#include <stdlib.h>
#include "sem.h"

int wc=0;
semaphore_t sem_lock,mutex;

typedef struct thread_args_s{
    int *var;
    semaphore_t *sem;
}thread_args_t;

void* worker(void *params)
{
    thread_args_t *temp = (thread_args_t*)params;
    while(1){
	P(temp->sem);
	P(&mutex);
	*(temp->var) = *(temp->var)+1;
	//printf ("Thread Function :: Start \n");
	//printf("value %d\n",*(temp->var));
	wc++;
	//printf ("Thread Function :: End \n");
	if(wc==3)
	    V(&sem_lock);
	else
	    V(&mutex);
    }

    pthread_exit (NULL);
}

int main(int argc,  char *argv[])
{  
    int num_threads = 3;
    thread_args_t params[3];
    semaphore_t s_lock[3];
    int a[3] = {0,0,0};
    init_sem(&sem_lock,0);
    init_sem(&mutex,1);
    for(int i = 0; i < num_threads; i++)
    {
	init_sem(&s_lock[i],1);
	params[i].var = &a[i];
	params[i].sem = &s_lock[i];
	start_thread(worker, (void *)&params[i]);
    }
    while(1){
	P(&sem_lock);
    	printf("array values from main thread, a[0]=%d a[1]=%d a[2]=%d\n",a[0],a[1],a[2]);
	wc=0;
	V(&s_lock[0]);V(&s_lock[1]);V(&s_lock[2]);
	V(&mutex);
    }

    return 0;
}

