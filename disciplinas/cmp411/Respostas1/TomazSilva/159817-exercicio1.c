#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//declaracao do semaforo
sem_t semaforo;

//funcao que sera chamada pelas threads
void * funcao(void *vi){
   sem_wait(&semaforo);
   printf("Sou a thread %d\n", vi);
   sem_post(&semaforo);
}

int main(){
   int i;
   
   pthread_t thread[5];

   //inicializacao do semaforo
   sem_init(&semaforo, 0, 1);
    
	for(i=0; i < 5; i++){   
		pthread_create(&thread[i], NULL, funcao, (void *)i);
   }
   
	for(i=0; i < 5; i++){	
		pthread_join(thread[i], NULL);
	}   
	return 0;
}
