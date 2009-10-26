#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <pthread.h>
#include <semaphore.h>

void *funcao_thread(void *);

int main(){
	printf("main - inicio\n");
	
        //declaracao das threads de criacao
	pthread_t threadcria;
	pthread_t threadcria1;
	pthread_t threadcria2;
	pthread_t threadcria3;
	pthread_t threadcria4;

	//inicializacao da thread que cria threads escritores
   	pthread_create(&threadcria, NULL, funcao_thread, 0);
   	pthread_create(&threadcria1, NULL, funcao_thread, 1);
   	pthread_create(&threadcria2, NULL, funcao_thread, 2);
   	pthread_create(&threadcria3, NULL, funcao_thread, 3);
   	pthread_create(&threadcria4, NULL, funcao_thread, 4);

	//join das threads de criacao
   	pthread_join(threadcria, NULL);
	pthread_join(threadcria1, NULL); 
	pthread_join(threadcria2, NULL); 
	pthread_join(threadcria3, NULL); 
	pthread_join(threadcria4, NULL);  

	printf("main - fim\n");
	
}

void * funcao_thread(void * param ){
	printf("Eu sou a thread %d \n",param);
}
