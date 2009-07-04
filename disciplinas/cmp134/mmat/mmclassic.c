#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
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

	//printf ("P_%d despachou uma ordem de nr %d para P_%d\n",my_rank,nr,processo);
	
	free(message);
}


matriz receba_matriz (int processo) {
	matriz m;
	int ordem;
	int tamanho;
	float* message;
	int i,j;
	MPI_Status recv_status, rs;
	
	//printf ("P_%d esperando a resposta de P_%d\n",my_rank, processo);

	MPI_Recv(&ordem, 1, MPI_INT, processo, 100, MPI_COMM_WORLD, &recv_status);		
	tamanho = pot(ordem,2);	
	message = calloc (tamanho, sizeof(float));	
	MPI_Recv(message, tamanho, MPI_FLOAT, processo, 101, MPI_COMM_WORLD, &rs);
	
	//printf ("P_%d recebeu a matriz de P_%d\n",my_rank, processo);

	m = new_matrix(ordem);	
	
	for (i = 0 ; i < ordem ; i++) {
		for (j = 0 ; j < ordem ; j++) {
			E(m,i,j) = message[i + ordem*j];
		}
	}
	
	free (message);
				
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
			
	MPI_Send(&ordem, 1, MPI_INT, processo, 100, MPI_COMM_WORLD);		
	MPI_Send(message, tamanho, MPI_FLOAT, processo, 101, MPI_COMM_WORLD);	
	
	free (message);
}





matriz mult (matriz A, matriz B, int nr /* nivel da recursao */) {
	int i,j,k, ordem, x,y;
	matriz C;
	ordem = A.ordem;

	/*if (nr == 2)
		printf("NR %d\n", nr);
	*/
	if ( ordem > MIN_RANK ) {
		
		matriz A_11, A_12, A_21, A_22;
		part_matrix (A, &A_11, &A_12, &A_21, &A_22);
		
		matriz B_11, B_12, B_21, B_22;		
		part_matrix (B, &B_11, &B_12, &B_21, &B_22);
  
    matriz Ca[3][3]; //assim posso usar Ca[1][1] etc.
    matriz Cb[3][3];
		int irmao[8]; //assim posso referenciar o 7o. irmao de maneira mais legível (i = 0..7)
		
		for (i = 1 ; i <= 7 ; i++)
			irmao[i] = acha_irmao(nr + 1,i);
		
		
		if (irmao[1])
			despache (irmao[1], A_11, B_11, nr + 1);
		else
			Ca[1][1] = mult (A_11, B_11, nr + 1);		
		
		if (irmao[2])
			despache (irmao[2], A_12, B_21, nr + 1);
		else
			Cb[1][1] = mult (A_12 , B_21, nr + 1);
			
		if (irmao[3])
			despache (irmao[3], A_11, B_12, nr + 1);
		else
			Ca[1][2] = mult (A_11,  B_12, nr + 1);
		
		if (irmao[4])
			despache (irmao[4], A_12, B_22, nr + 1);
		else
			Cb[1][2] = mult (A_12, B_22, nr + 1);
		
		if (irmao[5])
			despache (irmao[5], A_21, B_11, nr + 1);
		else
			Ca[2][1] = mult (A_21 , B_11, nr + 1);
		
		if (irmao[6])
			despache (irmao[6], A_22, B_21, nr + 1);
		else
			Cb[2][1] = mult (A_22 ,  B_21, nr + 1);		
      
    if (irmao[7])
			despache (irmao[7], A_21, B_12, nr + 1);
		else
			Ca[2][2] = mult (A_21 , B_12, nr + 1);		
		
		Cb[2][2] = mult (A_22 ,  B_22, nr + 1);
		


    i = 1;
    for (x = 1 ; x <= 2 ; x++) {
      for (y = 1 ; y <= 2 ; y++) {
        if (irmao[i]) Ca[x][y] = receba_matriz (irmao[i]);
        if (irmao[i+1] && i < 7) Cb[x][y] = receba_matriz (irmao[i+1]);
        i += 2;
      }
    }

		matriz C_11 = Ca[1][1];
		acc(C_11, Cb[1][1]);
		free_matrix(Cb[1][1]);
		
		matriz C_12 = Ca[1][2];
    acc(C_12, Cb[1][2]);
		free_matrix(Cb[1][2]);
		
		matriz C_21 = Ca[2][1];
    acc(C_21, Cb[2][1]);
		free_matrix(Cb[2][1]);
		
		matriz C_22 = Ca[2][2];
		acc(C_22, Cb[2][2]);		
		free_matrix(Cb[2][2]);
		
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

	//printf ("P_%d esperando ordem\n", my_rank);
	
	MPI_Recv(&nr, 1, MPI_INT, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD, &recv_status);
	MPI_Recv(&ordem, 1, MPI_INT, MPI_ANY_SOURCE, 100, MPI_COMM_WORLD, &recv_status);
	tamanho = pot(ordem,2);
	message = calloc(tamanho * 2, sizeof(float));
	MPI_Recv(message, tamanho * 2, MPI_FLOAT, MPI_ANY_SOURCE, 101, MPI_COMM_WORLD, &recv_status);
	
	//printf ("P_%d recebeu uma ordem de mult do nr %d\n", my_rank, nr);
	
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
 	
//	printf("antes de inicializar\n");

	MPI_Init(&argc, &argv);

//	printf("depois de inicializar\n");
	 
 	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	
	int i,j,k;
	double x,y;
	matriz A, B, C;

	int ORDEM = atoi(argv[1]);
	
	if (!my_rank) {
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
		//printf ("\nAGORA SIM!\n\n");
		gettimeofday(&start, NULL);
		C = mult (A, A, 0);
		gettimeofday(&end, NULL);
		//print_matrix("C" , C);

		total_usec = (end.tv_sec - start.tv_sec)*1e6 + (end.tv_usec - start.tv_usec);

		printf ("%ld\n",total_usec);
	
	} else {
		int altura = my_log2 (ORDEM);
		int nramos = pot (8, altura - 1);

		//printf ("altura = %d\n", altura);

		if (my_rank < nramos) {
			//printf ("P_%d ira participar... %d < %d\n",my_rank, my_rank, nramos);
			espere_ordem();
		}
	}
	
 	MPI_Finalize();
	
	return 0;
}

