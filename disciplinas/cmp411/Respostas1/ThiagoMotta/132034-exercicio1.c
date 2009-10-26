#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMTHREADS 5

void *imprime(void *);

int main()
{
   pthread_t threads[NUMTHREADS]; 
   int i;
   
   //inicialização das threads
   for (i=0; i<NUMTHREADS; i++)
   {
      pthread_create(&threads[i], NULL, imprime, (void *)i);
   }

   //join das threads (sincronização com a thread principal)
   for (i=0; i<NUMTHREADS; i++){
      pthread_join(threads[i],NULL);
   }

}


//função que será executada pelas threads 
void *imprime(void *i)
{
	while (1)
		printf("Sou a thread %d. ", (int) i);
}

