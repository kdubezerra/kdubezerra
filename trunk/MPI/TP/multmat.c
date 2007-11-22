#include <stdio.h>

#define ORDEM 1000

unsigned int f (int i, int j) {

	return ( i + j ) % 3;

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
		}
	}

	printf ("AGORA SIM!\n");
	
	for (i = 0 ; i < ORDEM ; i++) {
		for (j = 0 ; j < ORDEM ; j++) {
			C[i][j] = 0;
			for (k = 0 ; k < ORDEM ; k++) {
				C[i][j] += A[i][k] * A[k][j];
			}
		}
	}

}
