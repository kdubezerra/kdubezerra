#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main (int argc, char *argv[]) {
int t_id;
#pragma omp parallel private(t_id)
  {
     t_id = omp_get_thread_num();
     printf("Oi mundo da thread %d\n", t_id);
  }  /* Sincronizacao */
  printf("Isso eh impresso por apenas uma thread.\n");
}

