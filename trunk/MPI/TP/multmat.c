#include <stdio.h>
#include <stdlib.h>

//#define ORDEM 2
#define E(X,i,j) (* el(X,i,j))

float f (int i, int j) {

	return (float) (/*( i + j )*/rand() % 5 + 1);

}

typedef struct _matriz {
	int soma;
	struct _matriz *M1, *M2;
	float** array;
	int ordem, start_x, start_y;	
} matriz;

float* el (matriz m, int i, int j) {
	static float* s = NULL;
	if (!(m.soma))
		return &(m.array[i + m.start_x][j + m.start_y]);
	else {
		if (s) free (s);			
		s = malloc(sizeof(float));
		if (m.soma == 1) {
			(*s) = E((*m.M1), i, j) + E((*m.M1), i ,j);
		}
		else {
			(*s) = E((*m.M1), i, j) - E((*m.M2), i ,j);
		}
		return s;
	}
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
	mat.soma = 0;
	
	return mat;
}

void free_matrix (matriz m) {
	int i;
	
	if (m.soma)
		return;
	
	for (i = 0 ; i < m.ordem ; i++)
		free (m.array[i]);
	
	free (m.array);
}


matriz mult (matriz A, matriz B) {
	int i,j,k, ordem;
	matriz C;
	ordem = A.ordem;
		
		C = new_matrix(ordem);
		
		for (i = 0 ; i < ordem ; i++) {
			for (j = 0 ; j < ordem ; j++) {
				E(C, i, j) = 0;
				for (k = 0 ; k < ordem ; k++) {
					E(C, i, j) += E(A, i, k) * E(B, k, j);
				}
				printf ("C[%d][%d] = %f\n",i,j,E(C, i,j));
			}
		}
		
		return C;	
	
}


int main (int argc, char** argv) {
	int i,j,k;
	double x,y;
	matriz A, B, C;
	
	int ORDEM = atoi(argv[1]);
	
	A = new_matrix(ORDEM);
	B = new_matrix(ORDEM);	
	
//	A = calloc(ORDEM*ORDEM, sizeof(unsigned int));
//	B = calloc(ORDEM*ORDEM, sizeof(unsigned int));
//	C = calloc(ORDEM*ORDEM, sizeof(unsigned int));

	printf ("PREENCHENDO\n");

	for (i = 0 ; i < ORDEM ; i++) {
		for (j = 0 ; j < ORDEM ; j++) {
			E(A, i, j) = f(i,j);
			printf ("A[%d][%d] = %f\n",i,j,E(A, i,j));
		}
	}

	printf ("\nAGORA SIM!\n\n");	

	C = mult (A, A);
	
	for (i = 0 ; i < ORDEM ; i++) {
		for (j = 0 ; j < ORDEM ; j++) {			
// 			printf ("C[%d][%d] = %f\n",i,j,E(C, i,j));
		}
	}
	
}
