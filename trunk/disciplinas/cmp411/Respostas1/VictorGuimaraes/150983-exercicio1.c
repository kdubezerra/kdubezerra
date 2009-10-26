#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//int pthread_create(pthread_t * thread, attributes* attr, void * (*function)(void *), void * arg)

//funcao que sera chamada pelas threads
void * funcao(void *indice){
   printf("Sou a thread, %d \n", indice);
}

int main(){
	
   //declaracao das threads. 
   pthread_t thread1, thread2, thread3, thread4,thread5;
  
   //inicializacao das threads passando como parametro para funcao o indice de criação das threads
   pthread_create(&thread1, NULL, funcao, (void *)1);
   pthread_create(&thread2, NULL, funcao, (void *)2);
   pthread_create(&thread3, NULL, funcao, (void *)3);
   pthread_create(&thread4, NULL, funcao, (void *)4);
   pthread_create(&thread5, NULL, funcao, (void *)5);

   pthread_join(thread1, NULL);
   pthread_join(thread2, NULL);
   pthread_join(thread3, NULL);
   pthread_join(thread4, NULL);
   pthread_join(thread5, NULL);
   printf("FIM \n");
   return 0;
}
