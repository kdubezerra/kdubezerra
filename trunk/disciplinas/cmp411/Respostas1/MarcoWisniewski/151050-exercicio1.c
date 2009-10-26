#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void printid(void * id) {
    printf("Sou a thread %d\n", id);
    return;
}

int main(void) {
   int i;
   
   pthread_t vthread[5];

   for (i = 0; i < 5; i++) 
	pthread_create(&vthread[i], NULL, printid, (void *) i);
   for (i = 0; i < 5; i++) 
	pthread_join(vthread[i], NULL);
   return 0;
}



