#define _GNU_SOURCE
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
#include <math.h>
#include <stdbool.h>

long long int loopcnt = 10000000 ; // result in 6 precision
int numCPU = -1 ; 
int partition ; // distribute work to cpu 

volatile double upArea[ 1024 ] ; 
volatile double lowArea[ 1024 ] ; 
double upSum = 0.0 , lowSum = 0.0  ; 
double unit ;

void calculate_pi( int id );
void thread( void *givenName );
void sumArea( double pi );

int main(int argc, char **argv)
{
    double pi = 0.0 ;
    if(argv[1] != NULL ) 
        numCPU = atoi(argv[1]) ; 
    else numCPU = sysconf( _SC_NPROCESSORS_ONLN );
    
    //printf("numCPU: %d\n", numCPU);
    pthread_t* tid = (pthread_t*)malloc(sizeof(pthread_t) * numCPU);
    for (long i=0; i< numCPU; i++)
    {
        pthread_create(&tid[i], NULL, (void *) thread, (void*)i);
    }   
    for (int i=0; i< numCPU; i++)
    {
        pthread_join(tid[i], NULL);
    }
    sumArea( pi ) ; 
    return 0 ; 
}

void thread( void *givenName )
{
    int id = (intptr_t)givenName ;
    calculate_pi( id ) ; 
}

void calculate_pi( int id )
{
    //printf("%d", id );
    partition = loopcnt / numCPU ; // distribute work to cpu 
    unit = (double)1 / (double) loopcnt ; // unit of the 1/4 circle
    int head = id * partition ;
    int tail = ( id+1 ) * partition ;
    double x = id*partition*unit;         
    double y = sqrt(1-x*x);
    for( int i = head+1 ; i <= tail ; i++ ) 
    {
        upArea[id] += y;
        x = i*unit , y = sqrt(1-x*x) ;  // don't compute again       
        lowArea[id] += y ;
    }
    //printf("%f, %f\n", upArea[id], lowArea[id]);
    upArea[id] = upArea[id]*unit ;
    lowArea[id] = lowArea[id]*unit ;
}

void sumArea( double pi )
{
    for(int i=0;i<numCPU;i++)
    {
        upSum += upArea[i];
        lowSum += lowArea[i];
    }
    upSum *= 4.0 ;
    lowSum *= 4.0 ;
    // limit upper and lower 
    //printf("上面的面積： %.11f\n",upSum);
    //printf("下面的面積： %.11f\n",lowSum);
    char upstring[15];
    char lowstring[15];
    sprintf(upstring,"%.11f",upSum);
    sprintf(lowstring,"%.11f",lowSum);
    int idx=0;
    char ans[15];
    while( upstring[idx]==lowstring[idx] )
    {
        ans[idx]=upstring[idx];
        idx++;
    }
    ans[idx]='\0';
    printf("pi的值 = 3.14159265359\n") ;  
    printf("上下逼近求出來的PI = %s\n" , ans ); //correct pi 
}