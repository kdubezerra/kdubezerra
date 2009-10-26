/* Cristiano Werner Araújo - 150124	
   Rafael Hansen da Silva  - 149748
reposta da questão 2a:
O problema relacionado a concorrência pode ocorrer quando uma operação de crédito ou uma operação de débito é interrompida quando é executa a soma ou subtração a variável global saldo. Existindo a possibilidade de gerar um valor inconsistente, por deixar uma variável em um estado intermediário e após ser alterado por outra thread. Esse problema pode ser resolvido ao acrescentar um semáforo, restringindo a utilização da variável saldo 1 uma thread por vez.


*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUMOPER 10
#define NUMTHREADS 3
//inicia o saldo
int saldo = 100;
sem_t mexe_no_saldo;
void creditar(int valor);
void debitar(int valor);
void *threadcreditar(void *);
void *threaddebitar(void *);

int main(){
   //declaração das threads que fazem débitos
   pthread_t threadsd[NUMTHREADS];
   //declaração das threads que fazem créditos
   pthread_t threadsc[NUMTHREADS];
   //inicializa o semáforo
   sem_init(&mexe_no_saldo,0,1);
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
   sem_wait(&mexe_no_saldo);
   int saldo_conta = saldo + valor;
   printf("Creditando valor. \n");
   printf("O novo valor sera %d. \n", saldo_conta);
   sleep(1);
   saldo = saldo_conta;
   sem_post(&mexe_no_saldo);
}

//procedimento que efetua débitos na conta(saldo)
void debitar(int valor){
   sem_wait(&mexe_no_saldo);
   int saldo_conta = saldo - valor;
   printf("Debitando valor. \n");
   printf("O novo valor sera %d. \n", saldo_conta);
   sleep(1);
   saldo = saldo_conta;
   sem_post(&mexe_no_saldo);
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
