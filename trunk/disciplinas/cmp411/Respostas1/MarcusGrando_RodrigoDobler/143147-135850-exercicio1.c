#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//declaracao do semaforo
sem_t semaforo;

//funcao que sera chamada pelas threads
void * funcao (void *vi){
  sem_wait(&semaforo);
  char *message;
     message = (char *) vi;
     printf("%s \n", message);
  sem_post(&semaforo);
}

int main(){
   char *message1 = "Thread 1";
   char *message2 = "Thread 2";
   char *message3 = "Thread 3";
   char *message4 = "Thread 4";
   char *message5 = "Thread 5";
   //declaracao das threads. Pode ser criado um array de threads
   //quando for necessario criar muitas threads ex. pthread_t threads[X]
   pthread_t thread1, thread2, thread3, thread4,thread5;

  //inicializacao do semaforo
  sem_init(&semaforo, 0, 1);
    
   //inicializacao das threads passando como parametro para funcao as variaveis i e j
   pthread_create(&thread1, NULL, funcao, (void *) message1);
   pthread_create(&thread2, NULL, funcao, (void *) message2);
   pthread_create(&thread3, NULL, funcao, (void *) message3);
   pthread_create(&thread4, NULL, funcao, (void *) message4);
   pthread_create(&thread5, NULL, funcao, (void *) message5);

   //fazendo o join das threads com o processo principal. 
   //Importante: lembrar que se o processo que chamou as threads terminar, 
   //as threads tambem terminam. Sempre fazer o join para nao ocorrer esse problema.
   pthread_join(thread1, NULL);
   pthread_join(thread2, NULL);
   pthread_join(thread3, NULL);
   pthread_join(thread4, NULL);
   pthread_join(thread5, NULL);
   printf("FIM \n");
   return 0;
}
