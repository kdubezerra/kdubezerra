#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define N 5

//declaracao do semaforo
sem_t semaforo;

//funcao que sera chamada pelas threads
//semáforo será utilizado para guardas a função printf que utiliza a saída padrão
void * funcao(void *vi){
   sem_wait(&semaforo);
   printf("Sou a thread %d \n", (int)vi);
   sem_post(&semaforo);
}

int main(){

   //cria lista de 5 threads
   pthread_t threads[N];

   //inicialização de um semáforo
   sem_init(&semaforo, 0, 1);
    
   //cria as thread
   int i=0;
   while(i<N){
   	pthread_create(&threads[i], NULL, funcao, (void *)i);
	i++;
   }

 
   // executa as threads
   i=0;
   while(i<N){
   	pthread_join(threads[i], NULL);
	i++;
   }
   return 0;
}
