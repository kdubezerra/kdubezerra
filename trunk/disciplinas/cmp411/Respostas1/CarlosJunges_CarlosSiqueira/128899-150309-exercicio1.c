#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
 

void *funcao(void *param)
{
       int id = (int)param;
               printf("Sou a thread %d\n", id);
       pthread_exit(NULL);
}
 
int main(void)
{
       pthread_t threads[5];
       int i;
       for (i = 0; i < 5; i++)
       {
               pthread_create(&threads[i], NULL, funcao, (void *)i);
       }
       for (i = 0; i < 5; i++)
       {
               pthread_join(threads[i], NULL);
       }
       return EXIT_SUCCESS;
}

