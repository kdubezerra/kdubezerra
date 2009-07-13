#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {
	ifstream entrada;
	ofstream saida;

	entrada.open (argv[1]);
	saida.open (argv[2]);

	

	while (!entrada.eof()) {
		int i;
		long time;
		double mw, ms, dev, ns, w[8], oh[8];
		double amw = 0, ams = 0, adev = 0, ans = 0, aw[8] = {0,0,0,0,0,0,0,0}, aoh[8] = {0,0,0,0,0,0,0,0};
		for (i = 0 ; i < 100 && !entrada.eof() ; i++) {
			entrada >> time >> mw >> ms >> dev >> ns >> w[0] >> w[1] >> w[2] >> w[3] >> w[4] >> w[5] >> w[6] >> w[7] >> oh[0] >> oh[1] >> oh[2] >> oh[3] >> oh[4] >> oh[5] >> oh[6] >> oh[7];
			amw += mw;
			ams += ms;
			adev += dev;
			ans += ns;
			for (int j = 0 ; j < 8 ; j++) {
				aw[j] += w[j];
				aoh[j] += oh[j];
			}	
		}
		saida << time << " " << amw << " " << ams << " " << adev / (double)(i) << " " << ans / (double)(i) << " " << aw[0] / (double)(i) << " " << aw[1] / (double)(i) << " " << aw[2] / (double)(i) << " " << aw[3] / (double)(i) << " " << aw[4] / (double)(i) << " " << aw[5] / (double)(i) << " " << aw[6] / (double)(i) << " " << aw[7] / (double)(i) << " " << aoh[0] / (double)(i) << " " << aoh[1] / (double)(i) << " " << aoh[2] / (double)(i) << " " << aoh[3] / (double)(i) << " " << aoh[4] / (double)(i) << " " << aoh[5] / (double)(i) << " " << aoh[6] / (double)(i) << " " << aoh[7] / (double)(i) << endl;
	}
}
