/* O problema está nas funções 'creditar(...)' e 'debitar(...)', as quais efetuam uma alteração em um buffer, dormem um pouco (a dormida
 * 	apenas piora as coisas, mas elas ocorreriam de qualquer maneira), e só depois atualizam o valor real do saldo. Assim, pode ocorrer que duas
 * 	ou mais treads alterem o mesmo valor, e que os resultados sejam sobreescritos.
 *
 *	Resolvemos isso com um semáforo inicializado com um (um mutex), que permite que apenas uma tread efetue qualquer alteraçõa em saldo (credito ou
 *		débito);
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMOPER 10
#define NUMTHREADS 3
//inicia o saldo
int saldo = 100;

sem_t mutex;

void creditar(int valor);
void debitar(int valor);
void *threadcreditar(void *);
void *threaddebitar(void *);

int main(int argc, char **argv){
	// Inicializa o semáforo com 1;
   sem_init(&mutex, 0, 1);
	
   //declaração das threads que fazem débitos
   pthread_t threadsd[NUMTHREADS];
   //declaração das threads que fazem créditos
   pthread_t threadsc[NUMTHREADS];
   
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
   int saldo_conta = saldo + valor;
   printf("Creditando valor. \n");
   printf("O novo valor sera %d. \n", saldo_conta);
   sleep(1);
   saldo = saldo_conta;
}

//procedimento que efetua débitos na conta(saldo)
void debitar(int valor){
   int saldo_conta = saldo - valor;
   printf("Debitando valor. \n");
   printf("O novo valor sera %d. \n", saldo_conta);
   sleep(1);
   saldo = saldo_conta;
}

//função que será executada pelas threads tipo threadsc
void * threadcreditar(void *dummy){
   int i;
   for (i=0;i<NUMOPER;i++){
		sem_wait(&mutex);
      creditar(100);
		sem_post(&mutex);
   }
}

//função que será executada pelas threads tipo threadsd
void * threaddebitar(void *dummy){
   int i;
   for (i=0;i<NUMOPER;i++){
		sem_wait(&mutex);
      debitar(100);
		sem_post(&mutex);
   }
}
