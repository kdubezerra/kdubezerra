#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 5

//funcao que sera chamada pelas threads
void * funcao(void *vi){
   printf("Sou a thread %d\n", (int) vi);
}

int main(){
    int i;
    pthread_t thread[5];
    
    for (i = 0; i < NUM_THREADS; i++)
          pthread_create(&thread[i], NULL, funcao, (void *)i);
    for (i = 0; i < NUM_THREADS; i++)
          pthread_join(thread[i], NULL);

    return 0;
}
