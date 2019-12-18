/* PRUDHVI GALI - 1213366386
   VAMSI KRISHNA LANKA - 1215371417 
   */
#include "threads.h"
#include <unistd.h>

int global=0;

void function_1()
{
    int local = 0;

    while (1){
	local++;global++;
	printf("Function 1 incremented .... global = %d  local = %d\n", global, local);
	yeild();
	sleep(1);
    }
}    

void function_2()
{
    int local = 0;

    while (1){
	local++;global++;
	printf("Function 2 incremented .... global = %d  local = %d\n", global, local);
	yeild();
	sleep(1);
    }
}    

void function_3()
{
    int local = 0;

    while (1){
	local++;global++;
	printf("Function 3 incremented .... global = %d  local = %d\n", global, local);
	yeild();
	sleep(1);
    }
}    


int main()
{  
    start_thread(function_1);
    start_thread(function_2); 
    start_thread(function_3);
    run();
    return 0;
}




