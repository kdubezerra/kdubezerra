#include <stdio.h>
#include <omp.h>

int tid, numt, depth;

int pot (int base, int expoente) {
  if (expoente == 0) return 1;
  else return base * pot (base, expoente - 1);
}

int log2ceil (int x) {
  int i=0,j=1;
  while (j < x) {
    j *= 2;
    i++;
  }
  return i;
}

void myfunc(int lvl, int id) {
  
//  int tid__ = omp_get_thread_num();
//  printf ("tid__ = %d\n", tid__);
//  printf ("Thread #%d :: Hallo aus lvl %d, id %d (total depth %d)\n", tid__, lvl, id, depth);

  if (id + pot(2, lvl) >= numt) {
    int j;
    int tid__ = omp_get_thread_num();
    printf ("tid__ = %d\n", tid__);
    printf ("Thread #%d :: Hallo aus lvl %d, id %d (total depth %d)\n", tid__, lvl, id, depth);
    for (j = 0; j < 32768; j++) {
      double d = (double) rand() / (double) rand();
    }
  }
  else {
    #pragma omp task
      myfunc(lvl + 1, id);
  
    #pragma omp task
      myfunc(lvl + 1, id + pot(2, lvl));
   
   #pragma omp taskwait
  }

}

void sayhi() {
  printf ("Hi! I am # %d\n", omp_get_thread_num());
}


int main (int argc, char* argv[]) {

  #pragma omp parallel
  {
//    #pragma omp single
    {
      numt = omp_get_num_threads();
      tid = omp_get_thread_num();
      printf ("Number of threads: %d\n", numt);
      depth = log2ceil(numt);
  //    myfunc(0,0);
      sayhi();    
    }
  }
}
