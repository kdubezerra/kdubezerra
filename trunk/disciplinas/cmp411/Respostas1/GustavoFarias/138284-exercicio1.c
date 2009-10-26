#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void * funcao(void *vi){
   printf("Sou a Thread %d\n", vi); // alterado
}

int main(){
   
   pthread_t thread1, thread2;
   pthread_t thread3, thread4, thread5; // incluido

   pthread_create(&thread1, NULL, funcao, (void *)1); // incluido o id da thread
   pthread_create(&thread2, NULL, funcao, (void *)2); // incluido o id da thread
   pthread_create(&thread3, NULL, funcao, (void *)3); // incluida chamada e "
   pthread_create(&thread4, NULL, funcao, (void *)4); // incluida chamada e  "
   pthread_create(&thread5, NULL, funcao, (void *)5); // incluida chamada e  "

   pthread_join(thread1, NULL);
   pthread_join(thread2, NULL);
   pthread_join(thread3, NULL); // incluido
   pthread_join(thread4, NULL); // incluido
   pthread_join(thread5, NULL); // incluido

   printf("FIM \n");
   return 0;

}
