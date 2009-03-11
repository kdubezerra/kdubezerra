#include <iostream>
#include <fstream>
#include <vector>

typedef struct {
  int x,y;
} vertex;

typedef progdata vector<vector<vertex>>

using namespace std;

int getdata (char* inputfile);

int main(int argc, char** argv) {
  ifstream input;
  ofstream output;
  
  vector<vector<vertex>> setlist = getdata (argv[1]);

}

vector<vector<vertex>> getdata (char* inputfile) {
  ifstream input;
  int setcount;
  int setindex = 0;
  input.open (inputfile);
  input >> setcount;

  vector<vector<vertex>> setlist;

  input.open (argv[1]);
  output.open (argv[2]);

  while (!input.eof() && setindex++ < setcount) {
    int vertexcount;
    int vertexindex = 0;
    input >> vertexcount;
    vector<vertex> dataset;

    while (!input.eof() && vertexindex++ < vertexcount) {
      vertex p;
      input >> p.x >> p.y;
      dataset.push_back(p);
    }

    setlist.push_back(dataset);
  }

  return setlist;
}