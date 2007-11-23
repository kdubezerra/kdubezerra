#include <stdio.h>
#include <stdlib.h>

#define MIN_RANK 2
#define E(X,i,j) (* el(X,i,j))

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
// 			printf("%s[%d][%d] = %f\n",name, i, j, E(m,i,j));
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




matriz mult (matriz A, matriz B) {
	int i,j,k, ordem;
	matriz C;
	ordem = A.ordem;
	
	if ( ordem > MIN_RANK ) {		
		
		matriz A_11, A_12, A_21, A_22;
		part_matrix (A, &A_11, &A_12, &A_21, &A_22);
		
		matriz B_11, B_12, B_21, B_22;		
		part_matrix (B, &B_11, &B_12, &B_21, &B_22);
		
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
		
		matriz M_1 = mult (S_1a, S_1b);
		matriz M_2 = mult (S_2 , B_11);
		matriz M_3 = mult (A_11,  D_3);
		matriz M_4 = mult (A_22,  D_4);
		matriz M_5 = mult (S_5 , B_22);
		matriz M_6 = mult (D_6 ,  S_6);
		matriz M_7 = mult (D_7 ,  S_7);
		
		free_matrix (S_1a);
		free_matrix (S_1b);
		free_matrix (S_2);
		free_matrix (D_3);
		free_matrix (D_4);
		free_matrix (S_5);
		free_matrix (D_6);
		free_matrix (S_6);
		free_matrix (D_7);
		free_matrix (S_7);
		
		matriz C_11 = sub (M_4, M_5);
		acc(C_11, M_1);
		acc(C_11, M_7);
		free_matrix(M_7);
		
		matriz C_12 = some (M_3, M_5);
		free_matrix(M_5);
		
		matriz C_21 = some (M_2, M_4);
		free_matrix(M_4);
		
		matriz C_22 = sub (M_1, M_2);		
		acc(C_22, M_3);		
		acc(C_22, M_6);
		free_matrix(M_1);
		free_matrix(M_2);
		free_matrix(M_3);
		free_matrix(M_6);
		
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
	int i,j,k;
	double x,y;
	matriz A, B, C;
	
	int ORDEM = atoi(argv[1]);
	
	A = new_matrix(ORDEM);
	B = new_matrix(ORDEM);	

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

