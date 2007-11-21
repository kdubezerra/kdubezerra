#include <stdio.h>



for (i = 1 ; i <= M ; i++) {
	
	int P; // numero de processos
	int rank;
	float DP[P], NORM[P];
	
	for (j = 1 ; j <= N/P ; j++) {
		DPrank += Uij * Uij;
	}
	
	NORMrank = sum of DPi, i = 1 .. P;
	NORMrank = sqrt(NORMrank);
	
	broadcast (NORMrank);
	
	DPrank = sqrt(DPrank);
	
	for (j = 1 ; j <= N ; j++) {
		Vij = Uij / DPrank;
	}
	
	
	
	
	for (k = i+1 ; k <= M ; k++) {
		//p.i. = 0
		for (j = 1 ; j <= N ; j++) { // ACHA O PI DE < Uk , Vk >
			//p.i. += Ukj * Vkj
		}
		for (j = 1 ; j <= N ; j++) { // FAZ Uk = Uk - PI*Vi
			//Ukj = Ukj - p.i.*Vij
		}
	}
	
}