#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//declaracao do semaforo
sem_t semaforo;

//funcao que sera chamada pelas threads
void * funcao(void * vi){
   sem_wait(&semaforo);
   printf("Sou a thread %d \n", vi);
   sem_post(&semaforo);
}

int main(){
   int i=10;
   int j=20;
   int k=0;


   //declaracao das threads. Pode ser criado um array de threads
   //quando for necessario criar muitas threads ex. pthread_t threads[X]
   pthread_t thread[5];

   //inicializacao do semaforo
   sem_init(&semaforo, 0, 1);
    
   //inicializacao das threads passando como parametro para funcao as variaveis i e j
   //pthread_create(&thread1, NULL, funcao, (void *)i);
   //pthread_create(&thread2, NULL, funcao, (void *)j);

   for(k=0;k<5;k++)
	pthread_create(&thread[k], NULL, funcao, (void *)k);

   //fazendo o join das threads com o processo principal. 
   //Importante: lembrar que se o processo que chamou as threads terminar, 
   //as threads tambem terminam. Sempre fazer o join para nao ocorrer esse problema.
   //pthread_join(thread1, NULL);
   //pthread_join(thread2, NULL);
   for(k=0;k<5;k++)
   	pthread_join(thread[k], NULL);
   	

   printf("FIM \n");
   return 0;
}
