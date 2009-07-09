#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define MIN_RANK 2
#define E(X,i,j) (* el(X,i,j))

int my_rank, nprocs;

int my_log2 (int x) {
	if (x == 2) return 1; 
	else return 1 + my_log2(x/2);
}

int pot (int base, int exp) {
	return (int) pow ((double) base, (double) exp);	
}

float f (int i, int j) {

	return (float) (( i + j ) % 3);

}

typedef struct _matriz {
	float** array;
	int ordem, start_x, start_y;	
} matriz;


float* el (matriz m, int i, int j) {
	return &(m.array[i + m.start_x][j + m.start_y]);
}


matriz new_matrix (int x) {
	int i;
	matriz mat;
	
	float** m = calloc(x, sizeof(float));	
	for (i = 0 ; i < x ; i++)
		m[i] = calloc(x, sizeof(float));
	
	mat.array = m;
	mat.ordem = x;
	mat.start_x = mat.start_y = 0;	
	
	return mat;
}


void print_matrix (char* name, matriz m) {
	int i , j;
	
	for (i = 0 ; i < m.ordem ; i++) {
		for (j = 0; j < m.ordem ; j++) {
 			printf("%s[%d][%d] = %f\n",name, i, j, E(m,i,j));
		}
	}
	printf("\n");
	
}


void acc (matriz A, matriz B) {
	int i,j;
	
	for (i = 0 ; i < A.ordem ; i++)
		for (j = 0 ; j < A.ordem ; j++)
			E(A,i,j) += E(B,i,j);
}


matriz some (matriz A, matriz B) {
	int i,j;
	matriz new_soma = new_matrix(A.ordem);
	
	for (i = 0 ; i < new_soma.ordem ; i++)
		for (j = 0 ; j < new_soma.ordem ; j++)
			E(new_soma, i, j) = E(A, i, j) + E(B,i,j);
	
	return new_soma;
}


matriz sub (matriz A, matriz B) {
	int i,j;
	
	matriz new_sub = new_matrix(A.ordem);
	
	for (i = 0 ; i < new_sub.ordem ; i++)
		for (j = 0 ; j < new_sub.ordem ; j++)
			E(new_sub, i, j) = E(A, i, j) - E(B,i,j);
	
	return new_sub;
}


void free_matrix (matriz m) {
	int i;
	for (i = 0 ; i < m.ordem ; i++)
		free (m.array[i]);
	
	free (m.array);
}


matriz merge_matrix (matriz a11, matriz a12, matriz a21, matriz a22) {
	int ordem = a11.ordem;
	int i,j;
	matriz m = new_matrix(2*ordem);
	
	for (i = 0 ; i < ordem ; i++) {
		for (j  = 0 ; j < ordem ; j++) {
			E(m,i,j) = E(a11,i,j);
			E(m,i,j+ordem) = E(a12,i,j);
			E(m,i+ordem,j) = E(a21,i,j);			
			E(m,i+ordem,j+ordem) = E(a22,i,j);
		}
	}
	
	return m;
}


void part_matrix (matriz A, matriz *a11, matriz *a12, matriz *a21, matriz *a22) {
	int meia_ordem = A.ordem / 2;	
	(*a11) = (*a12) = (*a21) = (*a22) = A;	
	a11->ordem = a12->ordem = a21->ordem = a22->ordem = meia_ordem;	
		
	a12->start_x = a22->start_x = meia_ordem + A.start_x;
	a21->start_y = a22->start_y = meia_ordem + A.start_y;	
}


int acha_irmao (int nivel_recursao, int irmao) { //nivel começa em 0
	int processo_irmao = my_rank;
	
	if (nivel_recursao) // diferente do nivel 0
		processo_irmao += irmao * pot(8,nivel_recursao - 1);
	
	if (processo_irmao >= nprocs)
		return 0;
	else
		return processo_irmao;		
}

			






matriz mult (matriz A, matriz B, int nr /* nivel da recursao */) {
	int ordem, x,y;
	matriz C;
	ordem = A.ordem;

  C = new_matrix(ordem);
  
  #pragma omp parallel
  {
    int i,j,k;
    
    int tid = omp_get_thread_num();
    int chunk_size = ordem / omp_get_num_threads();
    if (ordem % omp_get_num_threads()) chunk_size++;
    int chunk_start = tid * chunk_size;
    int chunk_end = chunk_start + chunk_size;
  
    for (i = chunk_start ; i < chunk_end && i < ordem ; i++) {
      for (j = 0 ; j < ordem ; j++) {
        E(C, i, j) = 0;
        for (k = 0 ; k < ordem ; k++) {
          E(C, i, j) += E(A, i, k) * E(B, k, j);
        }
      }
    }
  
  }
		
		return C;
	
}





matriz recursive_mult (matriz A, matriz B) {
	int i,j,k, ordem;
	matriz C;
	ordem = A.ordem;
  
  //int tid = omp_get_thread_num();
  
  //#pragma omp critical
  //printf ("Thread #%d\n", tid);
	
	if ( ordem > MIN_RANK ) {
		
		matriz A_11, A_12, A_21, A_22;
		part_matrix (A, &A_11, &A_12, &A_21, &A_22);
		
		matriz B_11, B_12, B_21, B_22;		
		part_matrix (B, &B_11, &B_12, &B_21, &B_22);
		
    matriz M_111, M_112, M_121, M_122, M_211, M_212, M_221, M_222;
    
		#pragma omp task shared(M_111)
    M_111 = recursive_multv2 (A_11, B_11);
		
    #pragma omp task shared(M_112)
    M_112 = recursive_multv2 (A_12, B_21);
		
    #pragma omp task shared(M_121)
    M_121 = recursive_multv2 (A_11, B_12);
		
    #pragma omp task shared(M_122)
    M_122 = recursive_multv2 (A_12, B_22);
		
    #pragma omp task shared(M_211)
    M_211 = recursive_multv2 (A_21, B_11);
		
    #pragma omp task shared(M_212)
    M_212 = recursive_multv2 (A_22, B_21);
		
    #pragma omp task shared(M_221)
    M_221 = recursive_multv2 (A_21, B_12);
		
    #pragma omp task shared(M_222)
    M_222 = recursive_multv2 (A_22, B_12);

    #pragma omp taskwait

		matriz C_11 = some (M_111, M_112);
		free_matrix(M_111);
		free_matrix(M_112);
		
		matriz C_12 = some (M_121, M_122);
		free_matrix(M_121);
		free_matrix(M_122);
				
		matriz C_21 = some (M_211, M_212);
		free_matrix(M_211);
		free_matrix(M_212);
		
		matriz C_22 = some (M_221, M_222);
		free_matrix(M_221);
		free_matrix(M_222);
		
		C = merge_matrix(C_11, C_12, C_21, C_22);
		free_matrix(C_11);
		free_matrix(C_12);
		free_matrix(C_21);
		free_matrix(C_22);		
		
		return C;
		
	}
		
	if (ordem <= MIN_RANK) {
		
		C = new_matrix(ordem);
		
		for (i = 0 ; i < ordem ; i++) {
			for (j = 0 ; j < ordem ; j++) {
				E(C, i, j) = 0;
				for (k = 0 ; k < ordem ; k++) {
					E(C, i, j) += E(A, i, k) * E(B, k, j);
				}
			}
		}		
		
		return C;		
	}	
}

matriz recursive_multv2 (matriz A, matriz B) {
	int i,j,k, ordem;
	matriz C;
	ordem = A.ordem;
  
  //int tid = omp_get_thread_num();
  
  //#pragma omp critical
  //printf ("Thread #%d\n", tid);
	
	if ( ordem > MIN_RANK ) {
		
		matriz A_11, A_12, A_21, A_22;
		part_matrix (A, &A_11, &A_12, &A_21, &A_22);
		
		matriz B_11, B_12, B_21, B_22;		
		part_matrix (B, &B_11, &B_12, &B_21, &B_22);
		
    matriz M_111, M_112, M_121, M_122, M_211, M_212, M_221, M_222;
    
	
    M_111 = recursive_mult (A_11, B_11);
		
    M_112 = recursive_mult (A_12, B_21);
		
    M_121 = recursive_mult (A_11, B_12);
		
    M_122 = recursive_mult (A_12, B_22);
		
    M_211 = recursive_mult (A_21, B_11);
		
    M_212 = recursive_mult (A_22, B_21);
		
    M_221 = recursive_mult (A_21, B_12);
		
    M_222 = recursive_mult (A_22, B_12);


		matriz C_11 = some (M_111, M_112);
		free_matrix(M_111);
		free_matrix(M_112);
		
		matriz C_12 = some (M_121, M_122);
		free_matrix(M_121);
		free_matrix(M_122);
				
		matriz C_21 = some (M_211, M_212);
		free_matrix(M_211);
		free_matrix(M_212);
		
		matriz C_22 = some (M_221, M_222);
		free_matrix(M_221);
		free_matrix(M_222);
		
		C = merge_matrix(C_11, C_12, C_21, C_22);
		free_matrix(C_11);
		free_matrix(C_12);
		free_matrix(C_21);
		free_matrix(C_22);		
		
		return C;
		
	}
		
	if (ordem <= MIN_RANK) {
		
		C = new_matrix(ordem);
		
		for (i = 0 ; i < ordem ; i++) {
			for (j = 0 ; j < ordem ; j++) {
				E(C, i, j) = 0;
				for (k = 0 ; k < ordem ; k++) {
					E(C, i, j) += E(A, i, k) * E(B, k, j);
				}
			}
		}		
		
		return C;		
	}	
}


int main (int argc, char** argv) {
 	
//	printf("antes de inicializar\n");

	//MPIInit(&argc, &argv);

//	printf("depois de inicializar\n");
	 
 	//MPIComm_rank(//MPICOMM_WORLD, &my_rank);
 	//MPIComm_size(//MPICOMM_WORLD, &nprocs);
	
	int i,j,k;
	double x,y;
	matriz A, B, C;

	int ORDEM = atoi(argv[1]);
	
	
  struct timeval start, end;
  long total_usec;

  A = new_matrix(ORDEM);
  B = new_matrix(ORDEM);		



  //printf ("PREENCHENDO\n");

  for (i = 0 ; i < ORDEM ; i++) {
    for (j = 0 ; j < ORDEM ; j++) {
      E(A, i, j) = f(i,j);
    }
  }

  //print_matrix("A" , A);
  gettimeofday(&start, NULL);
  //printf ("\nAGORA SIM!\n\n");
  #pragma omp parallel
  {
    #pragma omp single 
    {
      C = recursive_mult (A, A);
    }
  }
  gettimeofday(&end, NULL);
  //print_matrix("C" , C);

  total_usec = (end.tv_sec - start.tv_sec)*1e6 + (end.tv_usec - start.tv_usec);

  printf ("%ld\n",total_usec);
	
	return 0;
}

