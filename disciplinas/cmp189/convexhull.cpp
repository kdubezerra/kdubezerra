#include <iostream>
#include <fstream>
#include <vector>
#include <list>

using namespace std;

typedef struct str_vertex {
  int x,y;
  
  bool operator<(const str_vertex &other) {
    if (x < other.x) return true;
    if (x > other.x) return false;
    //if x == other.x, then:
    if (y < other.y) return true;
    else return false;
  }
  
  str_vertex operator-(str_vertex &other)
  {
    str_vertex temp;

    temp.x = x - other.x;
    temp.y = y - other.y;

    return temp;
  }
} vertex;

typedef list<vertex> vertexset;
typedef list<vertexset> progdata;
    


progdata getdata (char* inputfile);
progdata processdata(progdata &data);
vertexset findhull(vertexset &vset);
bool lastthreeok(vertexset &vset);
void formatoutput(vertexset &vset);
void putdata(char* outputfile, progdata &data);
void showdata(progdata &data);


int main(int argc, char** argv) {  
  progdata setlist = getdata (argv[1]);  
  //showdata(setlist);
  progdata hulllist = processdata(setlist);
  //putdata(argv[2], hulllist);
  showdata(hulllist);
}


progdata getdata (char* inputfilename) {
  ifstream input;
  int setcount;
  int setindex = 0;
  input.open (inputfilename);
  input >> setcount;

  progdata setlist;

  while (!input.eof() && setindex++ < setcount) {
    int vertexcount;
    int vertexindex = 0;
    input >> vertexcount;
    list<vertex> dataset;

    while (!input.eof() && vertexindex++ < vertexcount) {
      vertex p;
      input >> p.x >> p.y;
      dataset.push_back(p);
    }
    if (!dataset.empty())
      dataset.pop_front(); //remove the repeated vertex
    
    if (!input.eof()) {
      int minusone; // read the '-1'
      input >> minusone;
    }

    setlist.push_back(dataset);
  }
  input.close();
  return setlist;
}


void showdata(progdata &data) {
  cout << data.size() << endl;
  for (progdata::iterator itprog = data.begin() ; itprog != data.end() ; ) {
    cout << itprog->size() << endl;
    for (vertexset::iterator itset = (*itprog).begin() ; itset != (*itprog).end() ; itset++) {
      cout << itset->x << " " << itset-> y << endl;
    }
    if (++itprog!= data.end())
      cout << "-1" << endl;
  }
}


progdata processdata(progdata &data) {
  progdata hulllist;
  for (progdata::iterator it = data.begin() ; it != data.end() ; it++) {
    hulllist.push_back(findhull(*it));
  }
  return hulllist;
}


vertexset findhull(vertexset &vset) {

  vset.sort();
  vertexset tophull;
  for (vertexset::iterator it = vset.begin() ; it != vset.end() ; it++) {
    tophull.push_back(*it);
    while (tophull.size() >= 3 && !lastthreeok(tophull)) {
      vertexset::iterator hi = tophull.end();
      --hi;
      --hi;
      tophull.erase(hi);
    }
  }

  vset.reverse();
  vertexset bottomhull;
  for (vertexset::iterator it = vset.begin() ; it != vset.end() ; it++) {
    bottomhull.push_back(*it);
    while (bottomhull.size() >= 3 && !lastthreeok(bottomhull)) {
      vertexset::iterator hi = bottomhull.end();
      --hi;
      --hi;
      bottomhull.erase(hi);
    }
  }
  
  //removing the first and last vertices of the bottom hull:
  vertexset::iterator hi;
  if (!bottomhull.empty()) {  
    hi = bottomhull.end();
    --hi;
    bottomhull.erase(hi);
  }
  if (!bottomhull.empty()) {
    hi = bottomhull.begin();
    bottomhull.erase(hi);
  }
  
  //putting the top and bottom hulls together as the full convex hull for this vertex set
  vertexset fullhull;
  fullhull.insert (fullhull.end(),tophull.begin(),tophull.end());
  fullhull.insert (fullhull.end(),bottomhull.begin(),bottomhull.end());
  
  //sorting the vertices in the order required by the judge
  formatoutput(fullhull);
  
  return fullhull;
}


bool lastthreeok(vertexset &vset) {
  vertex v1, v2, v3;
  vertexset::iterator vi = vset.end();  
  v3 = *(--vi);
  v2 = *(--vi);
  v1 = *(--vi);
  
  vertex vet1 = v2 - v1;
  vertex vet2 = v3 - v2;
  
  //calculating the cross product
  
  int crossproduct = (vet1.x)*(vet2.y) - (vet1.y)*(vet2.x);
  
  return (crossproduct > 0);
}


void formatoutput(vertexset &vset) {
  if (vset.empty()) return;

  vertexset::iterator smallest = vset.begin();
  for (vertexset::iterator it = vset.begin() ; it != vset.end() ; it++) {
    if (it->y < smallest->y)
      smallest = it;
    else if (it->y == smallest->y && it->x < smallest->x)
        smallest = it;
  }
  
  if (smallest == vset.begin())
    return;
 
  vset.insert(vset.end(), vset.begin(), smallest);
  vset.erase(vset.begin(), smallest);
  vset.push_back(*smallest);
}