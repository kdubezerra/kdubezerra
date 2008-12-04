#include <stdio.h>
#include <math.h>
#include "fgeneral.h"


double mean_time[26][1025];
double deviation[26][1025];
	

// int b_log (int base, int x) { // Bottom(log(base,x))
// 	int exp;	
// 	for (exp = 0 ; pot(base,exp) <= x ; exp++);
// 	exp--;		
// 	return exp;	
// }


void load_values (void) {
	FILE* arq_valores;
	char buffer[50];
	int N,P;

	for (P = 1 ; P <= 25 ; P += 6) {
		sprintf(buffer, "experiment_%d.txt", P);
		arq_valores = fopen (buffer, "r");
		
		printf ("\tArquivo: %s\n", buffer);

		for (N = 2 ; N <= 1024 ; N *= 2) {
			fscanf(arq_valores, "%lf %lf", &(mean_time[P][N]), &(deviation[P][N]));
		}

		fclose (arq_valores);
	}



}


double plot (int P, int N) {	
	
	if (P == 1) {
		return (0 - 2224.8542f + 178.5531f*N - 3.3571f*pow(N,2) + 0.9406f*pow(N,2.8f));
	}
	
	if (P == 7) {
		return (6568.8634f + 53.2971f*N + 1.0074f*pow(N,2) + 0.1346f*pow(N,2.8f));
	}
	
	if (P == 13) {
		return (7363.1484f + 62.2530f*N + 1.0166f*pow(N,2) + 0.1346f*pow(N,2.8f));
	}
	
	if (P == 19) {
		return (8202.7343f + 67.5663f*N + 1.1432f*pow(N,2) + 0.1155f*pow(N,2.8f));
	}
	
	if (P == 25) {
		return (9042.0f + 72.88f*N + 1.270f*pow(N,2) + 0.09638f*pow(N,2.8f));
	}	
	
	return 0;
	
}



int main () {
	FILE* arq_saida[26];	
	int N,P;
	
	printf ("Carregando valores...\n");
	
	load_values();
	
	printf ("Carregados os valores.\n");
	
	printf ("Criando arquivos de saída...\n");
	
	for (P = 1 ; P <= 25 ; P += 6) {
		char buffer[50];
		sprintf (buffer, "plot_%d.txt", P);
		arq_saida[P] = fopen(buffer,"w");
	}
	
	printf ("Criados os arquivos de saída.\n");
	
//	printf ("bottom(log2(0)) = %d\n", b_log(2,0));
// 	printf ("pot(2,-1) = %d\n", pot(2,-1));
	
	for (P = 1 ; P <= 25 ; P += 6) {
		for (N = 0 ; N <= 1024 ; N++) {
//			printf ("Fazendo para o P = %d e N = %d\n", P, N);
			fprintf(arq_saida[P], "%d : %lf", N, plot(P, N));
// 			printf ("Imprimiu no arquivo plot_%d.txt\n", P);
			if (N > 1 && pot(2,b_log(2,N)) == N) {
				fprintf(arq_saida[P], " : %lf : %lf : %lf", mean_time[P][N], deviation[P][N], Total (N,N,P));
			}
// 			printf ("Passou pelo if\n");
			fprintf(arq_saida[P], "\n");
		}
	}	
	
}