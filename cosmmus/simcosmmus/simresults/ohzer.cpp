#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {
	ifstream entrada;
	ofstream saida;

	entrada.open (argv[1]);
	saida.open (argv[2]);

	long mw, ms, ns;
	double power[8], weight[8], oh[8];

	entrada >> mw >> ms >> ns >> power[0] >> power[0] >> power[0] >> power[0] >> power[0] >> power[0] >> power[0] >> power[0] >> weight[0] >> weight[0] >> weight[0] >> weight[0] >> weight[0] >> weight[0] >> weight[0] >> weight[0] >> oh[0] >>  oh[0] >> oh[0] >> oh[0] >> oh[0] >> oh[0] >> oh[0] >> oh[0];

	double total_overhead = 0.0f;
	double wtotal = 0.0f;

	for (short i = 0 ; i < 8 ; i++) {
	  wtotal += weight[i];

	}
	for (short i = 0 ; i < 8 ; i++) {
	  total_overhead += oh[i];
	}
	total_overhead /= wtotal;

	saida << "Overhead de " << argv[1] << " = " << total_overhead << endl;
	
	return 0;
}
