/*
   As funções creditar e debitar possuiam problemas relacionados a concorrência, pois acessam e modificam uma variável compartilhada (saldo). Como o valor de saldo é lido no inicio das funções e é usado no final para calcular o novo valor de saldo, todo o corpo dessas funções devem fazer parte da seção crítica, pois o valor de saldo não pode ser modificado por outra thread entre a leitura e a modificação de saldo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMOPER 10
#define NUMTHREADS 3

sem_t semaforo;

//inicia o saldo
int saldo = 100;

void creditar(int valor);
void debitar(int valor);
void *threadcreditar(void *);
void *threaddebitar(void *);

int main(){
   //declaração das threads que fazem débitos
   pthread_t threadsd[NUMTHREADS];
   //declaração das threads que fazem créditos
   pthread_t threadsc[NUMTHREADS];
   
   int i;

   sem_init(&semaforo, 0, 1);
   
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
   int saldo_conta;

   sem_wait(&semaforo);
      saldo_conta = saldo + valor;
      printf("Creditando valor. \n");
      printf("O novo valor sera %d. \n", saldo_conta);
      saldo = saldo_conta;
   sem_post(&semaforo);
   sleep(1);
}

//procedimento que efetua débitos na conta(saldo)
void debitar(int valor){
   int saldo_conta;

   sem_wait(&semaforo);
      saldo_conta = saldo - valor;
      printf("Debitando valor. \n");
      printf("O novo valor sera %d. \n", saldo_conta);
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
