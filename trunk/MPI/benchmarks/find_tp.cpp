#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdio.h>

using namespace std;

int main (int argc, char** argv) {
	long unsigned tempo=666;
	long unsigned valor[20];
	int linhas;
	double media;
	double desvio;
	char arq_name[255];
		
	string bli1, bli2, bli3, bli4, bli5;
	char buffer[255];

// 	if (argc != 3) {
// 		cout << "Uso: " << argv[0] << " <arquivo_com_valores> <numero de linhas>" << endl;
// 		exit (1298);
// 	}

	for (long N = 2 ; N <= 1024 ; N *= 2) {
	
		media = 0;
		desvio = 0;
		
		sprintf (arq_name,"tempo_1_%ld.txt",N);
		
		ifstream resultFile (arq_name);
		linhas = 10;
	
		for ( int linha = 0 ; linha < linhas ; linha++ ) {
			resultFile >> valor[linha];
			media += (double) valor[linha];
		}
	
		media /= linhas;
	
		for (int bli = 0 ; bli < linhas ; bli++) {
			desvio += pow (    ( (double) valor[bli] )    -    media , 2);
		}
	
		desvio /= linhas;
	
		desvio = sqrt (desvio);
	
		cout.precision(0);
	
		cout << arq_name << endl;
		cout << "Media: " << fixed << media << "s" << endl;
		cout << "Desvio: " << fixed << desvio << "s" << endl << endl;	
	
	}

	exit(0);
}
