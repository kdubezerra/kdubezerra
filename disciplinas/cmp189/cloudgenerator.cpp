#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

int main (int argc, char** argv) {
   ofstream output;
   int x, y;
   int pmatrix[512][512];

   output.open(argv[1]);
   output << "1" << endl;
   output << "30002" << endl;
   output << "10" << " " << "10" << endl;

   for (long i = 0 ; i < 30000 ; /*don't increment yet*/ ) {
      x = rand() % 512;
      y = rand() % 512;
      if (pmatrix[x][y] == 154)
         continue;
      pmatrix[x][y] = 154;
      output << x << " " << y << endl;
      i++;     
   }
   output << "10" << " " << "10" << endl;

   return 0;
}
