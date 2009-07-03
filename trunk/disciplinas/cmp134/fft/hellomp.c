#include <stdio.h>
#include <omp.h>

int tid, numt;

int pot (int base, int expoente) {
  if (expoente == 0) return 1;
  else return base * pot (base, expoente - 1);
}

void myfunc(int lvl, int id) {
  printf ("Thread #%d :: Hallo aus lvl %d, id %d\n", omp_get_thread_num(), lvl, id);  
  if (id + pot(2, lvl) >= 16384/*numt*/)
    return;


  myfunc(lvl + 1, id);

//  #pragma omp task
  { myfunc(lvl + 1, id + pot(2, lvl)); }

  //#pragma omp taskwait

}

void sayhi() {
  printf ("Hi! I am # %d\n", omp_get_thread_num());
}


int main () {

  
  #pragma omp parallel
  #pragma omp single
  {
    numt = omp_get_num_threads();
  	tid = omp_get_thread_num();
    printf ("Number of threads: %d\n", numt);
    myfunc(0,0);
    sayhi();    
  }
}
