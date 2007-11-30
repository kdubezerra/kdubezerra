#include <stdio.h>

double alpha = 755.6127f;
double beta = 0.1815f;
double tp = 0.193891;

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

int t_div (int a, int b) {
	int res = (int) (a/b);
	if (res*b == a)
		return res;
	else
		return res + 1;
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
					(7 - b_div( P - pot(7,b_log(7,P)) , pot(7,b_log(7,P)) ))
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
	
	
	
	
double Comm (int n, int N, int P) {
	
	if (P == 1) {
		return 0;
	}
	
	else if (P >= pot(7, b_log(2,N) - 1)) {
		if (n > 2)
			return Comm(n/2, N, P) + 12*alpha + 9*(beta*pot(n,2))/2;
		else if (n == 2)
			return 0;
	}
	
	else if (P == pot(7, b_log(7,P))) {
		if ((n > 2) && n > N/pot(2, b_log(7,P))) {
			return Comm(n/2, N, P) + 12*alpha + 9*(beta*pot(n,2))/2;
		}
		else 
			return 0;
	}
	
	else {
		if ((n > 2) && n > N/pot(2, b_log(7,P))) {
			return Comm(n/2, N, P) + 12*alpha + 9*(beta*pot(n,2))/2;
		}
		else if ((n > 2) && n == N/pot(2, b_log(7,P))) {
			return t_div( (P - pot(7,b_log(7,P))) , pot(7,b_log(7,P)) ) * (2*alpha + 3*beta*pot(n,2)/4);
		}
		else
			return 0;
	}
	
}



double Total (int n, int N, int P) {
	
	return Proc(n, N, P) * tp + Comm(n, N, P);
	
}


int main(int argc, char** argv) {
	int P,N;
	char arq_name[10];
//	P = atoi (argv[1]);
	
// sprintf(arq_name, "Proc%d.txt", P);
// printf ("Nome do arquivo: %s\n",arq_name);
	
//	printf ("Testes:\n\tB_Log7(7) = %d\n\tB_Log7(8) = %d\n\tB_Log2(8) = %d\n\tB_Log2(7) = %d\n", b_log(7,7), b_log(7,8), b_log(2,8), b_log(2,7));
	
//	FILE* saida = fopen(arq_name,"w");
	FILE* TvsP = fopen("TvsP.txt","w");
				
//	for (N = 2 ; N <= 1024 ; N *= 2) {
//		fprintf (saida, "%lf\n", Total(N,N,P));
//	}
	
	for (P = 1 ; P <= 500 ; P++) {
		fprintf (TvsP, /*"%d*/" %lf\n", Total(1024,1024,P));
	}
		
	
}
