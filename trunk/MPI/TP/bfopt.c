#include <stdio.h>


#define M(X,Y) _M[X][Y]

int M_comb[8][8];
int _M[8][9];


int nComb (int n, int Ma, int Mb, int Mc, int Md, int Me, int Mf, int Mg) {
	
	
	
}


int main () {
	int i,j,k;
	

	
 	for (i = 1 ; i <= 7 ; i++)
 		for (j = 1 ; j <= 8 ; j++)
			M(i,j) = 0;

	
	M(1,1) = M(1,4) = M(1,5) = M(1,8) = 1;
	
	M(2,3) = M(2,4) = M(2,5) = 1;
	
	M(3,1) = M(3,6) = M(3,8) = 1;
	
	M(4,4) = M(4,5) = M(4,7) = 1;
	
	M(5,1) = M(5,2) = M(5,8) = 1;
	
	M(6,1) = M(6,3) = M(6,5) = M(6,6) = 1;
	
	M(7,2) = M(7,4) = M(7,7) = M(7,8) = 1;
	
	
	for (i = 1 ; i <= 7 ; i++) {
		for (j = 1 ; j <= 7 ; j++) {
			M_comb[i][j] = 0;
		}
	}
	
	for (i = 1 ; i <= 7 ; i++) {
		for (j = 1 ; j <= 7 ; j++) {
			for (k = 1 ; k <= 8 ; k++) {
				if (M(i,k) || M(j,k))
					M_comb[i][j]++;
			}
		}
	}
	
	for (i = 1 ; i <= 7 ; i++)
		for (j = 1; j <= 7 ; j++)
			printf ("comb[%d][%d] = %d\n", i , j , M_comb[i][j]);
	
	
}
