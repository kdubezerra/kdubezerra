#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM 5

//funcao que sera chamada pelas threads
void * funcao(void *vi){
   printf("Sou a thread %d\n", vi);
}

int main(){
	int i = 0;

	pthread_t my_threads[NUM];
   
	for (i = 0; i < NUM; i++) {
		pthread_create(my_threads + i, NULL, funcao, (void*)i);
	}
 
   //fazendo o join das threads com o processo principal. 
   //Importante: lembrar que se o processo que chamou as threads terminar, 
   //as threads tambem terminam. Sempre fazer o join para nao ocorrer esse problema.
/*   pthread_join(thread1, NULL);
   pthread_join(thread2, NULL);**/
	
	for (i = 0; i < NUM; ++i) {
		pthread_join(my_threads[i], NULL);
	}

   printf("FIM \n");
   return 0;
}
