#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMOPER 10
#define NUMTHREADS 3
//inicia o saldo
int saldo = 100;

void creditar(int valor);
void debitar(int valor);
void *threadcreditar(void *);
void *threaddebitar(void *);

//declaracao do semaforo
sem_t semaforo;

int main(){
   //declaração das threads que fazem débitos
   pthread_t threadsd[NUMTHREADS];
   //declaração das threads que fazem créditos
   pthread_t threadsc[NUMTHREADS];

   sem_init(&semaforo, 0, 1);   

   int i;
   
   //inicialização das threads
   for (i=0; i<NUMTHREADS; i++){
      pthread_create(&threadsc[i], NULL, threadcreditar, NULL);
      pthread_create(&threadsd[i], NULL, threaddebitar, NULL);
   }

   //join das threads (sincronização com a thread principal)
   for (i=0; i<NUMTHREADS; i++){
      pthread_join(threadsc[i],NULL);
      pthread_join(threadsd[i],NULL);
   }
   
   printf("Saldo final na conta = %d \n", saldo);
}


//procedimento que efetua créditos na conta(saldo)
void creditar(int valor){
   sem_wait(&semaforo);
   int saldo_conta = saldo + valor;
   printf("Creditando valor. \n");
   printf("O novo valor sera %d. \n", saldo_conta);

   saldo = saldo_conta;
   sem_post(&semaforo);

   sleep(1);
}

//procedimento que efetua débitos na conta(saldo)
void debitar(int valor){
   sem_wait(&semaforo);
   int saldo_conta = saldo - valor;
   printf("Debitando valor. \n");
   printf("O novo valor sera %d. \n", saldo_conta);
 //  sleep(1);
   saldo = saldo_conta;
   sem_post(&semaforo);

   sleep(1);
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
