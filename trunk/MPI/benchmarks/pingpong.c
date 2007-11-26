#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <sys/time.h>

int my_rank, target_rank, nprocs;

long measure (int length) {
	
	struct timeval start, end;
	long total_usec;
	MPI_Status recv_status;
	
	float* sfoo = calloc (length, sizeof(float));
	float* rfoo = malloc (sizeof(float));
	
	gettimeofday(&start, NULL);
	MPI_Send(sfoo, length, MPI_FLOAT, target_rank, 100, MPI_COMM_WORLD);
	MPI_Recv(rfoo, 0, MPI_FLOAT, target_rank, 100, MPI_COMM_WORLD, &recv_status);
	gettimeofday(&end, NULL);
	free(sfoo);
	free(rfoo);
	
	total_usec = (end.tv_sec - start.tv_sec)*1e6 + (end.tv_usec - start.tv_usec);	
	return total_usec;
	
}


void cobaia (int length) {
	
	MPI_Status recv_status;
	float* rfoo = calloc (length, sizeof(float));
	float* sfoo = malloc (sizeof(float));
	
	MPI_Recv(rfoo, length, MPI_FLOAT, 0, 100, MPI_COMM_WORLD, &recv_status);
	MPI_Send(sfoo, 0, MPI_FLOAT, 0, 100, MPI_COMM_WORLD);
	
	free(rfoo);
	free(sfoo);
		
}


int main (int argc, char** argv) {
 	
//	printf("antes de inicializar\n");

	MPI_Init(&argc, &argv);

//	printf("depois de inicializar\n");
	 
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	
	int comprimento = atoi(argv[1]);
	target_rank = atoi(argv[2]);
	
	if (!my_rank) {		
		printf ("%d\n", measure(comprimento));	
	} else if (my_rank == target_rank) {
		cobaia(comprimento);
	}
	
	MPI_Finalize();
	
	return 0;
}

