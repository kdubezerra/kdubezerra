#include <stdio.h>



for (i = 1 ; i <= M ; i++) {
	
	 //p.i. = 0;
	
	for (j = 1 ; j <= N ; j++) {
		//p.i. += Uij * Uij
	}
	
	//p.i. = sqrt(p.i.);
	
	for (j = 1 ; j <= N ; j++) {
		//Vij = Uij / p.i.;
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