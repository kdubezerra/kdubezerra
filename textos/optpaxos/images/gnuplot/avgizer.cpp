#include <iostream>
#include <string>
#include <fstream>

#define REPETITIONS 100

using namespace std;

int main(int argc, char** argv) {
  if (argc < 2) exit(-1);
  ifstream input (argv[1]);
  ofstream output (argv[2]);
  
  for (int i = 0 ; i < 11 ; i++) {
    int nmsgs;
    int wp;
    double avg_mistakes = 0.0f;
    double avg_dropped = 0.0f;
    for (int j = 0 ; j < REPETITIONS ; j++) {
      int mistakes, dropped;
      input >> wp >> nmsgs >> mistakes >> dropped;
      avg_mistakes += (double) mistakes;
      avg_dropped += (double) dropped;
    }
    avg_mistakes /= (double) REPETITIONS;
    avg_dropped /= (double) REPETITIONS;
    output << wp << " " << nmsgs << " " << avg_mistakes << " " << avg_dropped << endl;
  }
}