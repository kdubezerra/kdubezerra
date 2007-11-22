#include <stdio.h>

#define ORDEM 2

unsigned int f (int i, int j) {

	return ( i + j ) % 3;

}

void multiplique (unsigned int A[][ORDEM], unsigned int B[][ORDEM], unsigned int C[][ORDEM], int n, int desl_i, int desl_j) {
	int i,j,k;
	
	if ( n > 2)
		return;
		
	if (n == 2)
	
		for (i = desl_i ; i < desl_i + n ; i++) {
			for (j = desl_j ; j < desl_j + n ; j++) {
				C[i][j] = 0;
				for (k = 0 ; k < ORDEM ; k++) {
					C[i][j] += A[i][k] * A[k][j];
				}
			}
		}
	
}

int main () {
	int i,j,k;
	double x,y;
	unsigned int A[ORDEM][ORDEM], C[ORDEM][ORDEM];
	
//	A = calloc(ORDEM*ORDEM, sizeof(unsigned int));
//	B = calloc(ORDEM*ORDEM, sizeof(unsigned int));
//	C = calloc(ORDEM*ORDEM, sizeof(unsigned int));

	printf ("PREENCHENDO\n");

	for (i = 0 ; i < ORDEM ; i++) {
		for (j = 0 ; j < ORDEM ; j++) {
			A[i][j] = f(i,j);
			printf ("A[%d][%d] = %d\n",i,j,A[i][j]);
		}
	}

	printf ("\nAGORA SIM!\n\n");

	multiplique (A, C, ORDEM, 0, 0);
	
	for (i = 0 ; i < ORDEM ; i++) {
		for (j = 0 ; j < ORDEM ; j++) {
			C[i][j] = 0;
			for (k = 0 ; k < ORDEM ; k++) {
				C[i][j] += A[i][k] * A[k][j];				
			}
			printf ("C[%d][%d] = %d\n",i,j,C[i][j]);
		}
	}

	
	
}
