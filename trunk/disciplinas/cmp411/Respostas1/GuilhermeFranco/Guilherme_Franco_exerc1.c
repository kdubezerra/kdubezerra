/*Questão 1
 *Guilherme Fachini e Franco
 */

#include <pthread.h>
#include <stdio.h>
#define NUM     5

void *Print(void *threadid)
{
   long tid;
   tid = (long)threadid;
   printf("Sou a thread %ld!\n", tid);
   pthread_exit(NULL);
}

int main (int argc, char *argv[])
{
   pthread_t threads[NUM];
   int rc;
   long t;
   for(t=0; t<NUM; t++){
      rc = pthread_create(&threads[t], NULL, Print, (void *)t);
   }
   pthread_exit(NULL);
}

