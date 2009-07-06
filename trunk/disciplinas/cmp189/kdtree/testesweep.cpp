#include <iostream>
#include <stdlib.h>
#include <list>
#include <SDL/SDL.h>
#include "myutils.h"

#define DEFAULT_NUM   10
#define DEFAULT_WIDTH 100
#define DEFAULT_REACH 10
#define DEFAULT_THRESHOLD 3

using namespace std;

int width, num, reach, threshold;
void calc_weight_sortxandy ();

typedef struct {
  int x,y,dx,dy;
} point;

bool compareX (point a, point b) {
  return (a.x < b.x);
}

bool compareY (point a, point b) {
  return (a.y < b.y);
}

void fill_list(list<point>& avs) {  
  for (int i = 0 ; i < num ; i++) {
    point newpoint;
    newpoint.x = rand() % width;
    newpoint.y = rand() % width;
    newpoint.dx = (float)(rand() % 10000) / 10000.0f;
    newpoint.dy = (float)(rand() % 10000) / 10000.0f;
    avs.push_back(newpoint);
    cout << "(" << newpoint.x << ", " << newpoint.y << ")" << endl;
  }
}

long calc_relevance(point a, point b) {  
  float dist = distance (a.x , a.y , b.x , b.y);                        
  if (dist < threshold) {
    return 100;
  }
  if (dist < reach && belongsToVisibility(a.x, a.y, a.dx, a.dy, b.x, b.y)) {
    return approx(convertToScale(dist, threshold, reach, 0, 100));
  }                        
  return 0;
}

void calc_weight_bruteforce (list<point> avs) {
  int count = 0;
  long time;
  
  cout << "\nFazendo o algoritmo brute-force" << endl;
  
  time = SDL_GetTicks();
  for (list<point>::iterator itext = avs.begin() ; itext != avs.end() ; itext++) {
    for (list<point>::iterator itint = avs.begin() ; itint != avs.end() ; itint++) {
      calc_relevance(*itext, *itint);
      ///cout << "O ponto (" << itext->x << ", "<< itext->y << ") foi comparado com o ponto (" << itint->x << ", " << itint->y << ")" << endl;
      count++;
    }
  }
  time = SDL_GetTicks() - time;
  
  cout << "Número de comparações: " << count << endl;
  cout << "Tempo: " << time << endl;
}

void calc_weight_sortx (list<point> avs) {
  int count = 0;
  long time;
  avs.sort(compareX);
  list<point>::iterator beginint = avs.begin();
  
  cout << "\nFazendo o sweep em X apenas" << endl;
  
  time = SDL_GetTicks();
  for (list<point>::iterator itext = avs.begin() ; itext != avs.end() ; itext++) {
    for (list<point>::iterator itint = beginint ; itint != avs.end() ; itint++) {
      if (itint->x < (itext->x - reach)) {
        beginint++;
        continue;
      }
      if (itint->x > (itext->x + reach)) {
        break;
      }
      calc_relevance(*itext, *itint);
      ///cout << "O ponto (" << itext->x << ", "<< itext->y << ") foi comparado com o ponto (" << itint->x << ", " << itint->y << ")" << endl;
      count++;
    }
  }
  time = SDL_GetTicks() - time;
  
  cout << "Número de comparações: " << count << endl;
  cout << "Tempo: " << time << endl;
  
}

void calc_weight_sortxandy (list<point> avs) {
  long time;
  int count = 0;
  avs.sort(compareX);
  list<point>::iterator beginint = avs.begin();
  list<point> sel;
  
  cout << "\nFazendo o sweep em X e Y" << endl;
  
  time = SDL_GetTicks();
  for (list<point>::iterator itext = avs.begin() ; itext != avs.end() ; itext++) {
    sel.clear();
    for (list<point>::iterator itint = beginint ; itint != avs.end() ; itint++) {
      if (itint->x < (itext->x - reach)) {
        beginint++;
        continue;
      }
      if (itint->x > (itext->x + reach)) {
        break;
      }
      sel.push_back(*itint);
      
    }
    sel.sort(compareY);
    for (list<point>::iterator ity = sel.begin() ; ity != sel.end() ; ity++) {
      if (ity->y < (itext->y - reach)) continue;
      if (ity->y > (itext->y + reach)) break;
      calc_relevance(*itext, *ity);
      ///cout << "O ponto (" << itext->x << ", "<< itext->y << ") foi comparado com o ponto (" << ity->x << ", " << ity->y << ")" << endl;
      count++;
    }
  }
  time = SDL_GetTicks() - time;
  
  cout << "Número de comparações: " << count << endl;
  cout << "Tempo: " << time << endl;
  
}








int main(int argc, char** argv) {  
  srand(time(NULL));
  
  int SDL_Init(SDL_INIT_EVERYTHING);
  
  cout << "Usage: " << argv[0] << " [num [width [reach [threshold]]]]" << endl;
  
  reach = DEFAULT_REACH;
  width = DEFAULT_WIDTH;
  num = DEFAULT_NUM;
  threshold = DEFAULT_THRESHOLD;
  
  if (argc > 1) {
    num = atoi(argv[1]);
    if (argc > 2) {
      width = atoi(argv[2]);
      if (argc > 3) {
        reach = atoi(argv[3]);
        if (argc > 4) {
          threshold = atoi(argv[4]);
        }
      }
    }
  }
  
  list<point> avlist;
  
  fill_list(avlist);
  
  calc_weight_bruteforce(avlist);
  calc_weight_sortx(avlist);
  calc_weight_sortxandy(avlist);
  
  void SDL_Quit();
}
