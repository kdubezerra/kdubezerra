#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//declaracao do semaforo
sem_t semaforo;

//funcao que sera chamada pelas threads
void * funcao(int vi){
   
   sem_wait(&semaforo);
   printf("Sou a Thread %d\n", vi);
   sem_post(&semaforo);
}

int main(){
   int i;
   //declaracao das threads. Pode ser criado um array de threads
   //quando for necessario criar muitas threads ex. pthread_t threads[X]
   pthread_t thread1, thread2,thread3,thread4,thread5;

   //inicializacao do semaforo
   sem_init(&semaforo, 0, 1);
    
   //inicializacao das threads passando como parametro para funcao as variaveis i e j
   
   pthread_create(&thread1, NULL, funcao, 1);
   pthread_create(&thread2, NULL, funcao, 2);
   pthread_create(&thread3, NULL, funcao, 3);
   pthread_create(&thread4, NULL, funcao, 4);
   pthread_create(&thread5, NULL, funcao, 5);
  

  return 0;
}
