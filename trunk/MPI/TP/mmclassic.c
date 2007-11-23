#include <stdio.h>
#include <stdlib.h>

#define MIN_RANK 2

//#define ORDEM 2
#define E(X,i,j) (* el(X,i,j))

float f (int i, int j) {

	return (float) (( i + j ) % 3);

}

typedef struct _matriz {
	float** array;
	int ordem, start_x, start_y;	
} matriz;

float* el (matriz m, int i, int j) {
// 	static float* s = NULL;
// 	if (!(m.soma))
	return &(m.array[i + m.start_x][j + m.start_y]);
// 	else {
// 		if (s) free (s);			
// 		s = malloc(sizeof(float));
// 		if (m.soma == 1) {
// 			(*s) = E((*m.M1), i, j) + E((*m.M1), i ,j);
// 		}
// 		else {
// 			(*s) = E((*m.M1), i, j) - E((*m.M2), i ,j);
// 		}
// 		return s;
// 	}
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
	
	if (A.ordem != B.ordem) {
		//printf ("erro: somando matrizes de ordens diferentes\n");
	}
	
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
// 	return suboradd(A, B, 1);
}

matriz sub (matriz A, matriz B) {
	int i,j;
	
	matriz new_sub = new_matrix(A.ordem);
	
	for (i = 0 ; i < new_sub.ordem ; i++)
		for (j = 0 ; j < new_sub.ordem ; j++)
			E(new_sub, i, j) = E(A, i, j) - E(B,i,j);
	
	return new_sub;
	
// 	return suboradd(A, B, -1);
}

// matriz suboradd (matriz A, matriz B, int addorsub) {
// 	matriz s;
// 	
// 	s.soma = addorsub;
// 	s.M1 = &A;
// 	s.M2 = &B;
// 	s.ordem = A.ordem; // ou B.ordem, tanto faz...
// 	
// 	return s;
// 	
// }

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




matriz mult (matriz A, matriz B) {
	int i,j,k, ordem;
	matriz C;
	ordem = A.ordem;
	
	//printf ("Recursão da multiplicação, ordem %d\n", ordem);
	
	if ( ordem > MIN_RANK ) {		
		
		matriz A_11, A_12, A_21, A_22;
		part_matrix (A, &A_11, &A_12, &A_21, &A_22);
		
		matriz B_11, B_12, B_21, B_22;		
		part_matrix (B, &B_11, &B_12, &B_21, &B_22);
		
		matriz M_111 = mult (A_11, B_11);
		matriz M_112 = mult (A_12, B_21);
		matriz M_121 = mult (A_11, B_12);
		matriz M_122 = mult (A_12, B_22);
		matriz M_211 = mult (A_21, B_11 );
		matriz M_212 = mult (A_22, B_21);
		matriz M_221 = mult (A_21, B_12);
		matriz M_222 = mult (A_22, B_12);

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
		
		//printf ("Deu free nas Cij's\n");
		
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
// 				printf ("C[%d][%d] = %f\n",i,j,E(C, i,j));
			}
		}
		
		print_matrix("C", C);
		
		return C;
		
	}
	
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
		}
	}
	
	print_matrix("A" , A);

	printf ("\nAGORA SIM!\n\n");	

	C = mult (A, A);
	
	print_matrix("C" , C);
	
}

