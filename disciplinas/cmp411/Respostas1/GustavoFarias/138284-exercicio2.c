#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMOPER 10
#define NUMTHREADS 3

sem_t semaforo; // incluido

int saldo = 100;

void creditar(int valor);
void debitar(int valor);
void *threadcreditar(void *);
void *threaddebitar(void *);

int main(){
   sem_init(&semaforo, 0, 1); // inicializado semáforo

   pthread_t threadsd[NUMTHREADS];
   pthread_t threadsc[NUMTHREADS];
   int i;
   for (i=0; i<NUMTHREADS; i++){
      pthread_create(&threadsc[i], NULL, threadcreditar, NULL);
      pthread_create(&threadsd[i], NULL, threaddebitar, NULL);
   }
   for (i=0; i<NUMTHREADS; i++){
      pthread_join(threadsc[i],NULL);
      pthread_join(threadsd[i],NULL);
   }
   
   printf("Saldo final na conta = %d \n", saldo);
}


void creditar(int valor){
   sem_wait(&semaforo); // incluida entrada no semaforo
     int saldo_conta = saldo + valor;
     printf("Creditando valor. \n");
     printf("O novo valor sera %d. \n", saldo_conta);
     saldo = saldo_conta;
     sleep(1);
   sem_post(&semaforo); // incluida saída no semaforo
}

void debitar(int valor){
   sem_wait(&semaforo); // incluida entrada no semaforo
     int saldo_conta = saldo - valor;
     printf("Debitando valor. \n");
     printf("O novo valor sera %d. \n", saldo_conta);
     saldo = saldo_conta;
     sleep(1);
   sem_post(&semaforo); // incluida saída no semaforo
}

//função que será executada pelas threads tipo threadsc
void * threadcreditar(void *dummy){
   int i;
   for (i=0;i<NUMOPER;i++){
      creditar(100);
   }
}

//função que será executada pelas threads tipo threadsd
void * threaddebitar(void *dummy){
   int i;
   for (i=0;i<NUMOPER;i++){
      debitar(100);
   }
}
