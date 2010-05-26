#include <iostream>
#include <list>
#include "myutils.h"


using namespace std;

int main () {
  
  cout << convertToScale(70, 0, 100, 0, 10) << endl;
  cout << convertToScale(70, 0, 100, 1000, 2000) << endl;
  cout << convertToScale(70, 0, 750, 0, 15) << endl;  
  cout << convertToScale(90, 0, 750, 0, 15) << endl;
  cout << convertToScale(50, 0, 750, 0, 15) << endl;
  cout << int(convertToScale(70, 0, 750, 0, 15)) << endl;  
  cout << int(convertToScale(90, 0, 750, 0, 15)) << endl;
  cout << int(convertToScale(50, 0, 750, 0, 15)) << endl << endl;
  
  cout << int(simpleScale(70, 750, 15)) << endl;  
  cout << int(simpleScale(90, 750, 15)) << endl;
  cout << int(simpleScale(50, 750, 15)) << endl << endl << endl;
  
  list<int> l;
  
  for (int i = 0 ; i < 5 ; i++)
    l.push_back(i);
  
  list<int> j;
  
//   l.merge(j);
  j.insert (j.begin(), l.begin(), l.end());
//   l.insert (l.begin(), l.begin(), l.end());
  
  for (list<int>::iterator it = l.begin() ; it != l.end() ; it++)
    cout << *it << endl;
  
  cout << endl;
  
  for (list<int>::iterator it = j.begin() ; it != j.end() ; it++)
    cout << *it << endl;
//   cout << normalizeToScale(0, 100, 75) << endl;
//   cout << normalizeToScale(50, 100, 70) << endl;
//   cout << normalizeToScale(0, 750, 70) << endl;
  
}
