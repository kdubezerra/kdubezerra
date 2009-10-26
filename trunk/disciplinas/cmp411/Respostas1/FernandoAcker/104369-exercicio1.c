#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMTHREADS 5

void * imprimeThread(void *);

int main(){
   //declaração das 5 threads
   pthread_t threads[NUMTHREADS];

   int  i;
   
   //inicialização das 5 threads
   for (i=0; i<NUMTHREADS; i++){
      pthread_create(&threads[i], NULL, &imprimeThread, i);
   }

}

void * imprimeThread(void * x){
   printf("Sou a thread %i \n", (int *)x);
   return;   
}
