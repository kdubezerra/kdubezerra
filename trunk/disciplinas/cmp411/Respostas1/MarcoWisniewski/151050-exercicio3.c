#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_OPS 2
#define NUM_ESCRITORES 3
#define NUM_LEITORES 3

//inicia o saldo
int saldo = 0;

//inicia a variavel que conta o numero de leitores lendo
int numero_leitores=0;

//semaforo para garantir exclusao mutua no acesso a variavel numero_leitores
sem_t sema_num_l;

//semaforo para garantir exclusao mutua no acesso a variavel saldo
sem_t sema_saldo;

// variavel para manter leitores vivos
int finaliza = 1;

void *escritor(void *);
void *leitor(void *);
void *crialeitor(void *);
void *criaescritor(void *);

int main(){
   int i;
   //declaracao das threads de criacao
   pthread_t threadcrials;
   pthread_t threadcriaes;

   //inicializacao dos semaforos
   sem_init(&sema_num_l, 0, 1);
   sem_init(&sema_saldo, 0, 1);
 
   //inicializacao da thread que cria threads escritores
   pthread_create(&threadcriaes, NULL, criaescritor, NULL);

   //inicializacao da thread que cria threads leitores
   pthread_create(&threadcrials, NULL, crialeitor, NULL);
   
   //join das threads de criacao
   pthread_join(threadcriaes, NULL);   
   finaliza = 0;
   pthread_join(threadcrials, NULL);   
   
   printf("Saldo final na conta = %d \n", saldo);
}

//funcao que sera executada pelas threads escritor
void * escritor(void * id){
   int novo_saldo;
   int idp = (int)(int *)id;
   printf("Escritor %d aguardando acesso.\n" , id);
   sem_wait(&sema_saldo);
      printf("Escritor %d iniciando alteracao.\n", id );
      novo_saldo = saldo + (10 * idp);
      sleep(2);
      saldo = novo_saldo;
      printf("Escritor %d alteracao Finalizada.\n", id);
   sem_post(&sema_saldo);
}

//funcao que sera executada pelas threads leitor
void * leitor(void * id){
  sem_wait(&sema_num_l);
    if (numero_leitores++ == 0) {
      printf("Leitor %d é o primeiro, entrando na fila\n", id);
      sem_wait(&sema_saldo);
    } else {
      printf("Leitor %d não é o primeiro, prosseguindo\n", id);
    }
  sem_post(&sema_num_l);
    
  printf("Leitor %d leu saldo = %d. \n", id, saldo);
  sleep(1);
  
  sem_wait(&sema_num_l);
    if (--numero_leitores == 0) {
      sem_post(&sema_saldo);
    } 
  sem_post(&sema_num_l);
}

//funcao que sera executada pelas threads leitor
void * criaescritor(void * id){
   int i;
   int nops = NUM_OPS;
   //declaracao das threads leitores
   pthread_t threadse[NUM_ESCRITORES];
   
   while(nops > 0){
      //inicializacao das threads leitores
      for (i=0; i < NUM_ESCRITORES; i++){
         pthread_create(&threadse[i], NULL, escritor, (void *)i);
      }
 
      for (i=0; i < NUM_ESCRITORES; i++){
         pthread_join(threadse[i], NULL);
      }
      sleep(1);
      nops--;
   }
}


//funcao que sera executada pelas threads leitor
void * crialeitor(void * id){
   int i;
   //declaracao das threads leitores
   pthread_t threadsl[NUM_LEITORES];
   
   while(finaliza){
      //inicializacao das threads leitores
      for (i=0; i < NUM_LEITORES; i++){
         pthread_create(&threadsl[i], NULL, leitor, (void *)i);
      }
 
      for (i=0; i < NUM_LEITORES; i++){
         pthread_join(threadsl[i], NULL);
      }
      sleep(1);
   }
}
