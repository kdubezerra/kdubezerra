#include <stdio.h>
#include <stdlib.h>

//#define ORDEM 2
#define E(X,i,j) (* el(X,i,j))

float f (int i, int j) {

	return (float) (( i + j ) % 3);

}

typedef struct _matriz {
	int soma;
	struct _matriz M1, M2;
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
			(*s) = E(m.M1, i, j) + E(m.M2, i ,j);
		}
		else {
			(*s) = E(m.M1, i, j) - E(m.M2, i ,j);
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

matriz some (matriz A, matriz B) {
	return suboradd(A, B, 1);
}

matriz sub (matriz A, matriz B) {
	return suboradd(A, B, -1);
}

matriz suboradd (matriz A, matriz B, int addorsub) {
	matriz s;
	
	s.soma = addorsub;
	s.M1 = A;
	s.M2 = B;
	s.ordem = A.ordem; // ou B.ordem, tanto faz...
	
	return matriz
	
}

void free_matrix (matriz m) {
	int i;
	
	for (i = 0 ; i < matriz.ordem ; i++)
		free (m.array[i]);
	
	free (m.array);
}



void mult (matriz A, matriz B, matriz C, int n, int min_rank) {
	int i,j,k;
	
	if ( n > min_rank) {
		matriz A_11, A_12, A_21, A_22;
		A_11 = A_12 = A_21 = A_22 = A; //soma e array são os mesmos
		
		matriz B_11, B_12, B_21, B_22;
		B_11 = B_12 = B_21 = B_22 = B; //soma e array são os mesmos
		
		A_12.start_x = B_12.start_x = A_21.start_y = B_21.start_y =	A_11.ordem = A_12.ordem = A_21.ordem = A_22.ordem = B_11.ordem = B_12.ordem = B_21.ordem = B_22.ordem = A.ordem / 2;
		
				
		
		matriz S_1a = some (A_11, A_22);
		matriz S_1b = some (B_11, B_22);
		matriz S_2 = some (A_21, A_22);
		matriz D_3 = sub (B_12, B_22);
		matriz D_4 = sub (B_21, B_11);
		matriz S_5 = some (A_11, A_12);
		matriz D_6 = sub (A_21, A_11);
		matriz S_6 = some (B_11, B_12);
		matriz D_7 = sub (A_12, A_22);
		matriz S_7 = some (B_21, B_22);
		
		
	}
		
	if (n <= min_rank)	
		for (i = 0 ; i < n ; i++) {
			for (j = 0 ; j < n ; j++) {
				E(C, i, j) = 0;
				for (k = 0 ; k < n ; k++) {
					E(C, i, j) += E(A, i, k) * E(B, k, j);
				}
				printf ("C[%d][%d] = %f\n",i,j,E(C, i,j));
			}
		}
	
}


int main (int argc, char** argv) {
	int i,j,k;
	double x,y;
	matriz A, B, C;
	
	int ORDEM = atoi(argv[1]);
	
	A = new_matrix(ORDEM);
	B = new_matrix(ORDEM);
	C = new_matrix(ORDEM);	
	
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

	multiplique (A, A, C, ORDEM, 2);	
	
}
