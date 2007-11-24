#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define MIN_RANK 2
#define E(X,i,j) (* el(X,i,j))

int my_rank, nprocs;

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
		processo_irmao += irmao * pot(7,nivel_recursao - 1);
	
	if (processo_irmao >= nprocs)
		return 0;
	else
		return processo_irmao;		
}

			
void despache (int processo, matriz A, matriz B, int nr) {
	int ordem = A.ordem;
	int tamanho = pot(ordem,2);
	int i,j;
	float* message = calloc (2*tamanho, sizeof (float));	
	
	for (i = 0 ; i < ordem ; i++) {
		for (j = 0 ; j < ordem ; j++) {
			message[i + ordem*j] = E(A,i,j);
			message[tamanho + i + ordem*j] = E(B,i,j);
		}
	}	
	
	MPI_Send(&nr, 1, MPI_INTEGER, processo, 99, MPI_COMM_WORLD);
	MPI_Send(&ordem, 1, MPI_INT, processo, 100, MPI_COMM_WORLD);
	MPI_Send(message, 2*tamanho, MPI_FLOAT, processo, 101, MPI_COMM_WORLD);
	
	free(message);
}


matriz receba_matriz (int processo) {
	matriz m;
	int ordem;
	int tamanho;
	float* message;
	int i,j;
	MPI_Status recv_status;
		
	MPI_Recv(&ordem, 1, MPI_INT, processo, 100, MPI_COMM_WORLD, &recv_status);
		
	tamanho = pot(ordem,2);
	
	printf ("P%d recebeu a ordem (%d) da matriz...\n", my_rank, ordem);
	
	MPI_Recv(message, tamanho, MPI_FLOAT, processo, 101, MPI_COMM_WORLD, &recv_status);
	
	printf ("P%d recebeu a matriz...\n", my_rank);
	
	m = new_matrix(ordem);
	
	
	
	for (i = 0 ; i < ordem ; i++) {
		for (j = 0 ; j < ordem ; j++) {
			E(m,i,j) = message[i + ordem*j];
		}
	}
	
	
	
	return m;
	
}


void envie_matriz (int processo, matriz m) {	
	int i,j;
	int ordem = m.ordem;
	int tamanho = pot(ordem,2);
	float* message = calloc(tamanho, sizeof(float));
	
	for (i = 0 ; i < ordem ; i++) {
		for (j = 0 ; j < ordem ; j++) {
			message[i + ordem*j] = E(m,i,j);
		}
	}
	
	print_matrix("m",m);
	
	MPI_Send(&ordem, 1, MPI_INT, processo, 100, MPI_COMM_WORLD);	
	
	MPI_Send(message, tamanho, MPI_FLOAT, processo, 101, MPI_COMM_WORLD);	
	
	free (message);
}





matriz mult (matriz A, matriz B, int nr /* nivel da recursao */) {
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
		
		matriz M[8];
		int irmao[7];
		
		
		for (i = 1 ; i <= 6 ; i++)
			irmao[i] = acha_irmao(nr + 1,i);
		
		
		if (irmao[1])			
			despache (irmao[1], S_1a, S_1b, nr + 1);
		else
			M[1] = mult (S_1a, S_1b, nr + 1);
		
		printf ("Despachou OK\n");
		
		if (irmao[2])
			despache (irmao[2], S_2, B_11, nr + 1);
		else
			M[2] = mult (S_2 , B_11, nr + 1);
			
		if (irmao[3])
			despache (irmao[3], A_11, D_3, nr + 1);
		else
			M[3] = mult (A_11,  D_3, nr + 1);
		
		if (irmao[4])
			despache (irmao[4], A_22, D_4, nr + 1);
		else
			M[4] = mult (A_22,  D_4, nr + 1);
		
		if (irmao[5])
			despache (irmao[5], S_5, B_22, nr + 1);
		else
			M[5] = mult (S_5 , B_22, nr + 1);
		
		if (irmao[6])
			despache (irmao[6], D_6, S_6, nr + 1);
		else
			M[6] = mult (D_6 ,  S_6, nr + 1);		
		
		M[7] = mult (D_7 ,  S_7, nr + 1);
		
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
		
		for (i = 1 ; i < 7 && irmao[i] ; i++) {
			M[i] = receba_matriz (irmao[i]);
		}
		
		matriz C_11 = sub (M[4], M[5]);
		acc(C_11, M[1]);
		acc(C_11, M[7]);
		free_matrix(M[7]);
		
		matriz C_12 = some (M[3], M[5]);
		free_matrix(M[5]);
		
		matriz C_21 = some (M[2], M[4]);
		free_matrix(M[4]);
		
		matriz C_22 = sub (M[1], M[2]);		
		acc(C_22, M[3]);		
		acc(C_22, M[6]);
		free_matrix(M[1]);
		free_matrix(M[2]);
		free_matrix(M[3]);
		free_matrix(M[6]);
		
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


void espere_ordem () {
	matriz A, B, C;
	int ordem, tamanho, nr;	
	int i,j;
	float* message;
	MPI_Status recv_status;
	
	MPI_Recv(&nr, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &recv_status);
	MPI_Recv(&ordem, 1, MPI_INT, MPI_ANY_SOURCE, 100, MPI_COMM_WORLD, &recv_status);
	
	tamanho = pot(ordem,2);
	
	MPI_Recv(message, tamanho * 2, MPI_FLOAT, MPI_ANY_SOURCE, 101, MPI_COMM_WORLD, &recv_status);
	
	A = new_matrix(ordem);
	B = new_matrix(ordem);
		
	for (i = 0 ; i < ordem ; i++) {
		for (j = 0 ; j < ordem ; j++) {
			E(A,i,j) = message[i + ordem*j];
			E(B,i,j) = message[tamanho + i + ordem*j];
		}
	}	
	
	free(message);	
		
	C = mult(A,B,nr);
		
	free_matrix(A);
	free_matrix(B);
		
	envie_matriz(recv_status.MPI_SOURCE, C);	
	
	free_matrix(C);
}


matriz cmult (matriz A, matriz B) {
	int i,j,k, ordem;
	matriz C;
	ordem = A.ordem;	
	
	if ( ordem > MIN_RANK ) {		
		
		matriz A_11, A_12, A_21, A_22;
		part_matrix (A, &A_11, &A_12, &A_21, &A_22);
		
		matriz B_11, B_12, B_21, B_22;		
		part_matrix (B, &B_11, &B_12, &B_21, &B_22);
		
		matriz M_111 = cmult (A_11, B_11);
		matriz M_112 = cmult (A_12, B_21);
		matriz M_121 = cmult (A_11, B_12);
		matriz M_122 = cmult (A_12, B_22);
		matriz M_211 = cmult (A_21, B_11);
		matriz M_212 = cmult (A_22, B_21);
		matriz M_221 = cmult (A_21, B_12);
		matriz M_222 = cmult (A_22, B_12);

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
 	
	
	MPI_Init(&argc, &argv);
	 
 	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	
	int i,j,k;
	double x,y;
	matriz A, B, C;
	
	if (!my_rank) {
	
	int ORDEM = atoi(argv[1]);
	
	A = new_matrix(ORDEM);
	B = new_matrix(ORDEM);		
	
	
	
	printf ("PREENCHENDO\n");
	
	for (i = 0 ; i < ORDEM ; i++) {
		for (j = 0 ; j < ORDEM ; j++) {
			E(A, i, j) = f(i,j);
		}
	}
	
	//print_matrix("A" , A);
	printf ("\nAGORA SIM!\n\n");	
	C = mult (A, A, 0);	
	//print_matrix("C" , C);
	
	} else {
		espere_ordem();
	}
	
 	MPI_Finalize();
	
	return 0;
}

