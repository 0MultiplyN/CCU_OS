#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/syscall.h> 
#include <assert.h>
#include <string.h>
#include <time.h>


int start = 0 ; 
int num_thr , critical_section_size , noncritical_section_size , num_iter = 0 ; 

pthread_spinlock_t lock ; 
struct timespec myt = { 0 , 0 } ;
atomic_int in_cs = 0 ;
atomic_long count_array[256];

void func( void* givenName ) 
{
    int givenID = (intptr_t)givenName;
    while (start == 0 ); // spin ; 
    
    for ( int i=0 ; i < num_iter ; i++  ) 
    {
    	
        pthread_spin_lock( &lock ) ;
        
        atomic_fetch_add(&in_cs, 1); // 將in_cs的值+1，代表進入critical section的thread數量增加1
        atomic_fetch_add_explicit(&count_array[givenID], 1, memory_order_relaxed); // 此CPU進入critical section的次數加1
        if (in_cs != 1) { // 判斷spinlock是否正常運作，正常運作下，每次只會有剛好1個thread可以進入critical section
            printf("violation: mutual exclusion\n"); // 印出錯誤訊息
            exit(0); // 結束程式
        }
        
        //nanosleep( critical_section_size ) ;  
        myt.tv_nsec = critical_section_size ; 
        nanosleep( &myt , NULL );
        //unlock();
        atomic_fetch_add(&in_cs, -1); // 將in_cs的值-1，代表進入critical section的thread數量減少1
        pthread_spin_unlock( &lock ) ; 
        int ncs = random()%noncritical_section_size;
        myt.tv_nsec = ncs ; 
        nanosleep( &myt , NULL ) ; 
        //nanosleep(ncs);
    }
}



int main(int argc, char **argv) 
{
    
    num_thr = atoi(argv[1]) ;
    critical_section_size = atoi(argv[2]) ; 
    noncritical_section_size = atoi( argv[3] ) ; 
    num_iter = atoi( argv[4] )  ; 
    
    //printf("waiting for child threads\n");
    pthread_t* tid = (pthread_t*)malloc(sizeof(pthread_t) * num_thr) ;
    pthread_spin_init( &lock , PTHREAD_PROCESS_PRIVATE ) ; 
    
    //printf("waiting for child threads\n");
    
    for (long i=0; i< num_thr; i++)
        pthread_create(&tid[i],NULL,(void *) func , (void*)i);

    start = 1 ; 
    
    for (int i=0; i< num_thr; i++)
	    pthread_join(tid[i],NULL);
	    
    //printf("all threads finish their work\n");
   
    
} 



