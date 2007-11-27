#include <stdio.h>
#include <stdlib.h>

char* P (int n) {
	char* string = calloc(20, sizeof(char));

	if (n > 1)
		sprintf (string, " (7x%s + 18x(%d)**2) ", P (n/2) , n/2);
	else
		sprintf (string, "1");

	return string;
}

int P1 (int n) {

	if (n > 2)
		return (7*P1(n/2) + 18*(n/2)*(n/2));
	else
		return 8;

}


int P7 (int n, int N) {
	
	if (n > N/2 && n > 2)
		return (P7(n/2, N) + 18*(n/2)*(n/2));
	else if (n <= N/2 && n>2)
		return (7*P7(n/2, N) + 18*(n/2)*(n/2));
	else if (n == 2)
		return 8;

}


int P13 (int n, int N) {

	return P7(n, N); // pois pelo menos 1 processo no segundo nivel terá que executar tudo sozinho. como o tempo é igual ao tempo do mais lento, tem-se que P13 = P7
	
// 	if (n	> N/2)
// 		return (P13(n/2, N) + 18*(n/2)*(n/2));
// 	else if (n == N/2 && n > 2)
// 		return (

}


int P19 (int n, int N) {
	
	if (n > N/2 && n > 2)
		return (P19(n/2, N) + 18*(n/2)*(n/2));
	else if (n == N/2 && n > 2)
		return (6*P19(n/2, N) + 18*(n/2)*(n/2));
	else if (n > 2 && n < N/2)
		return (7*P19(n/2, N) + 18*(n/2)*(n/2));
	else if (n == 2)
		return 8;
}

int P25 (int n, int N) {
	
	if (n > N/2 && n > 2)
		return (P25(n/2, N) + 18*(n/2)*(n/2));
	else if (n == N/2 && n > 2)
		return (5*P25(n/2, N) + 18*(n/2)*(n/2));
	else if (n > 2 && n < N/2)
		return (7*P25(n/2, N) + 18*(n/2)*(n/2));
	else if (n == 2)
		return 8;
}


int main (int argc, char** argv) {
	
	

	printf ("\nP1(2) = %d\n", P1( 2 ) );
	printf ("P1(4) = %d\n", P1( 4 ) );
	printf ("P1(8) = %d\n", P1( 8 ) );
	printf ("P1(16) = %d\n", P1( 16 ) );
	
	printf ("\nP7(2) = %d\n", P7( 2 , 2 ) );
	printf ("P7(4) = %d\n", P7( 4 , 4 ) );
	printf ("P7(8) = %d\n", P7( 8 , 8 ) );
	printf ("P7(16) = %d\n", P7( 16 , 16 ) );
	
	printf ("\nP13(2) = %d\n", P13( 2 , 2 ) );
	printf ("P13(4) = %d\n", P13( 4 , 4 ) );
	printf ("P13(8) = %d\n", P13( 8 , 8 ) );
	printf ("P13(16) = %d\n", P13( 16 , 16 ) );
	
	printf ("\nP19(2) = %d\n", P19( 2 , 2 ) );
	printf ("P19(4) = %d\n", P19( 4 , 4 ) );
	printf ("P19(8) = %d\n", P19( 8 , 8 ) );
	printf ("P19(16) = %d\n", P19( 16 , 16 ) );

	printf ("\nP25(2) = %d\n", P25( 2 , 2 ) );
	printf ("P25(4) = %d\n", P25( 4 ,4 ) );
	printf ("P25(8) = %d\n", P25( 8 , 8 ) );
	printf ("P25(16) = %d\n", P25( 16 ,16 ) );
	
}
