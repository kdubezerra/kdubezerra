/* 
   
   Disciplina: Algoritmos Geométricos
   Professor: João Luiz Dihl Comba
   Aluno: Carlos Eduardo Benevides Bezerra
   
   Tarefa: Em uma faixa no espaço dividida em regiões por segmentos,
   calcular, em O(log(n)), qual a região onde um ponto está situado.
   
   Descrição da solução:
   Os segmentos são armazenados em um vetor, ordenados pela sua
   posição. Para encontrar a região a que um ponto pertence em tempo
   O(log(n)), é feita uma busca binária neste vetor, até encontrar o
   par de segmentos que o cercam - ou, no caso de o ponto estar em
   uma das extremidades, qual o segmento mais próximo.
   
*/ 

#include <iostream>
#include <fstream>
#include <vector>
#include <list>

//for some reason, these defines didn't work in a long expression
//#define MAX(A,B) A>B?A:B
//#define MIN(A,B) A<B?A:B

using namespace std;

double MAX(double A, double B) {
  if (A>B) return A;
  else return B;
}

double MIN(double A, double B) {
  if (A<B) return A;
  else return B;
}

typedef struct strseg {
  double xb, xu;
  bool operator<(const strseg &other) {
    if (xu < other.xu) return true;
    if (xb < other.xb) return true;
    return false;
  }
} segment;

typedef struct strvertex {
  
  double x, y;
  
  bool b4seg(segment s) { //vertex is situated before segment s (from left to right)
    double comparable_x;
    if (MIN(s.xb, s.xu) == s.xb) comparable_x = MIN(s.xb, s.xu) + y * (MAX(s.xb, s.xu) - MIN(s.xb, s.xu));
    else comparable_x = MIN(s.xb, s.xu) + (1-y) * (MAX(s.xb, s.xu) - MIN(s.xb, s.xu));
    return (x < comparable_x);
  }
  
  bool afterseg(segment s) { //vertex is situated after segment s (from left to right)
    double comparable_x;
    if (MIN(s.xb, s.xu) == s.xb) comparable_x = MIN(s.xb, s.xu) + y * (MAX(s.xb, s.xu) - MIN(s.xb, s.xu));
    else comparable_x = MIN(s.xb, s.xu) + (1-y) * (MAX(s.xb, s.xu) - MIN(s.xb, s.xu));
    return (x > comparable_x);
  }
  
} vertex;

void getSegmentList(vector<segment> &slist);
void getVerticesList(vector<vertex> &vlist);
int findRegion(vertex v, vector<segment> &slist);
void locateVerticesRegions(vector<vertex> &vlist, vector<segment> &slist);

int main () {
  vector<segment> seglist;
  vector<vertex> vertexlist;
  getSegmentList(seglist);
  getVerticesList(vertexlist);
  locateVerticesRegions(vertexlist, seglist);
}

void getSegmentList(vector<segment> &slist) {
  list<segment> sorter;
  int segcount;
  segment s;
  cin >> segcount;
  for (int i = 0 ; i < segcount ; i++) {
    cin >> s.xb >> s.xu;
    sorter.push_back(s);
  }
  sorter.sort();
  slist.insert(slist.end(), sorter.begin(), sorter.end());
}

void getVerticesList(vector<vertex> &vlist) {
  int vertexcount;
  vertex v;
  cin >> vertexcount;
  for (int i = 0 ; i < vertexcount ; i++) {
    cin >> v.x >> v.y;
    vlist.push_back(v);
  }
}

int findRegion(vertex v, vector<segment> &slist) {
  if (slist.size() == 1) return 0;
  int begin = 0;
  int end = slist.size() - 1;
  int current;
  while (1) {
    current = (begin + end) / 2;
    //cout << "B = " << begin << "\nC = " << current << "\nE = " << end << endl;
    
    if (v.b4seg(slist[current])) {
      if (current == 0) return 0;
      if (v.afterseg(slist[current-1])) return current;
      end = current - 1;
    }
    
    if (v.afterseg(slist[current])) {
      if (current == slist.size() - 1) return slist.size();
      if (v.b4seg(slist[current+1])) return current+1;
      begin = current + 1;
    }
  }
}

void locateVerticesRegions(vector<vertex> &vlist, vector<segment> &slist) {
  cout << vlist.size() << endl;
  for (int i = 0 ; i < vlist.size() ; i++) {
    cout << vlist[i].x << " " << vlist[i].y << " R" << findRegion(vlist[i], slist) << endl;
  }
}











