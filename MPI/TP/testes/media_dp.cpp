#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

int main (int argc, char** argv) {
	long unsigned tempo=666;
	long unsigned valor[20]; //somatorio de (Xi - Xmedio)**2
	double media = 0;
	double desvio = 0;
		
	string bli1, bli2, bli3, bli4, bli5;
	char buffer[255];

	if (argc == 1) {
		cout << "Porra diga o nome do arquivo caralho!!!" << endl;
		exit (1298);
	}

	ifstream resultFile (argv[1]);

	for ( int linha = 0 ; linha < 20 ; linha++ ) {
		resultFile >> valor[linha];
		media += valor[linha];
	}

	media /= 20;

	for (int bli = 0 ; bli < 20 ; bli++) {
		desvio += pow (    ( (double) valor[bli] )    -    media , 2);
	}

	desvio /= 20;

	desvio = sqrt (desvio);

	cout.precision(4);

	cout << argv[1] << endl;
	cout << "Media: " << fixed << media/1e6 << "s" << endl;
	cout << "Desvio: " << fixed << desvio/1e6 << "s" << endl << endl;

	exit(0);
}
