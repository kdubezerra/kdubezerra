#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N       10

int main (int argc, char *argv[]) {
int tid, i;
double a[N], b[N], c[N];

/* Inicializações */
for (i=0; i < N; i++)
  a[i] = b[i] = i * 1.0;
printf("Fim da inicializacao sequencial.\n");
/* Laço paralelizado */
#pragma omp parallel for 
   for (i=0; i<N; i++) {
       tid = omp_get_thread_num();
       c[i] = a[i] + b[i];
       if (tid == 0) printf("Fluxo 0 calculando i = %d.\n",i);
   }
printf("Fim da parte paralela.\n");
}


