#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//declaracao do semaforo
sem_t semaforo;

//funcao que sera chamada pelas threads
void * funcao(void *vi){
   //sem_wait(&semaforo);
   printf("Sou a thread %d \n", vi);
   //sem_post(&semaforo);
}

int main(){
   int x1=1;
   int x2=2;
   int x3=3;
   int x4=4;
   int x5=5;

   //declaracao das threads. Pode ser criado um array de threads
   //quando for necessario criar muitas threads ex. pthread_t threads[X]
   pthread_t thread1, thread2, thread3, thread4, thread5;

   //inicializacao do semaforo
   sem_init(&semaforo, 0, 1);
    
   //inicializacao das threads passando como parametro para funcao as variaveis i e j
   pthread_create(&thread1, NULL, funcao, (void *)x1);
   pthread_create(&thread2, NULL, funcao, (void *)x2);
   pthread_create(&thread3, NULL, funcao, (void *)x3);
   pthread_create(&thread4, NULL, funcao, (void *)x4);
   pthread_create(&thread5, NULL, funcao, (void *)x5);

   //fazendo o join das threads com o processo principal. 
   //Importante: lembrar que se o processo que chamou as threads terminar, 
   //as threads tambem terminam. Sempre fazer o join para nao ocorrer esse problema.
   

   pthread_join(thread1, NULL);
   pthread_join(thread2, NULL);
   pthread_join(thread3, NULL);
   pthread_join(thread4, NULL);
   pthread_join(thread5, NULL);
//   printf("FIM \n");
   return 0;
}
