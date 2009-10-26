#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

//funcao que sera chamada pelas threads
void * funcao(void *id){
   printf("Sou a thread %d\n", (int)id);
}

int main(){
   int i;

   //declaracao das threads. Pode ser criado um array de threads
   //quando for necessario criar muitas threads ex. pthread_t threads[X]
   pthread_t threads[5];

   //inicializacao das threads passando como parametro para funcao a variavel i
   for (i = 0; i < 5; i++){
      pthread_create(&threads[i], NULL, funcao, (void *)i);
   }
   
   //fazendo o join das threads com o processo principal. 
   //Importante: lembrar que se o processo que chamou as threads terminar, 
   //as threads tambem terminam. Sempre fazer o join para nao ocorrer esse problema.
   for (i = 0; i < 5; i++){
      pthread_join(threads[i], NULL);
   }
   return 0;
}
