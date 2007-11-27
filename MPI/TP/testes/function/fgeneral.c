#include <stdio.h>

int pot (int base, int exp) {
	if (exp == 0)
		return 1;
	else 
		return base*pot(base,exp-1);
}

int b_log (int base, int x) { // Bottom(log(base,x))
	int exp;	
	for (exp = 0 ; pot(base,exp) <= x ; exp++);
	exp--;		
	return exp;	
}

int b_div (int a, int b) {
	return ((int) (a / b));
}

int Proc(int n, int N, int P) {
	
	if (P == 1) { // 1 processador
		if (n > 2)
			return 7*Proc(n/2, N, P) + 18*pot(n/2,2);
		else if (n == 2)
			return 8;
	} 
	
	else if (P >= pot(7, b_log(2,N) - 1)) { // Número ilimitado de processadores
		if (n > 2)
			return Proc(n/2, N, P) + 18*pot(n/2,2);
		else if (n == 2)
			return 8;
	}
	
	else if (P == pot(7, b_log(7,P))) { // Número de processadores é potência de 7 -> P = pot (7,k), int k > 0
		if ((n > 2) && n > N/pot(2, b_log(7,P))) { // verificar se essa divisao de doubles nao da pau
			printf ( "%d / %d = %d\n", N, pot(2, b_log(7,P)), N/pot(2, b_log(7,P)) ); //é só olhar aqui...
			return Proc(n/2, N, P) + 18*pot(n/2,2);
		}
		else if ((n > 2) && n <= N/pot(2, b_log(7,P))) {
			return 7*Proc(n/2, N, P) + 18*pot(n/2,2);
		}
		else if (n == 2)
			return 8;
	}
	
	else { // Outros casos: P > 1 && P < numero de ramos && P != pot(7,k)
		if ((n > 2) && n > N/pot(2, b_log(7,P))) {
			return Proc(n/2, N, P) + 18*pot(n/2,2);
		}
		else if ((n > 2) && n == N/pot(2, b_log(7,P))) {
			return 
					(7 - b_div( P - pot(7,b_log(7,P)) , 7 ))
					*
					Proc(n/2, N, P)
					+ 18*pot(n/2,2);
		}
		else if ((n > 2) && n < N/pot(2, b_log(7,P))) {
			return 7*Proc(n/2, N, P) + 18*pot(n/2,2);
		}
		else if (n == 2) {
			return 8;
		}
	}
}
	
	


int main(int argc, char** argv) {
	int P,N;
	char arq_name[10];
	P = atoi (argv[1]);
	
	sprintf(arq_name, "Proc%d.txt", P);
	printf ("Nome do arquivo: %s\n",arq_name);
	
	printf ("Testes:\n\tB_Log7(7) = %d\n\tB_Log7(8) = %d\n\tB_Log2(8) = %d\n\tB_Log2(7) = %d\n", b_log(7,7), b_log(7,8), b_log(2,8), b_log(2,7));
	
	FILE* saida = fopen(arq_name,"w");
				
	for (N = 2 ; N <= 1024 ; N *= 2) {
		fprintf (saida, "%d\n", Proc(N,N,P));
	}	
	
}
