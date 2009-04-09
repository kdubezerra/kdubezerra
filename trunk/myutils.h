#pragma once

#include <limits.h>

#ifdef _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <list>
#include <map>
#include <signal.h>
#include <math.h>

#define uint64 unsigned long
#define nplayers WW
#define WW 750
// ww = world width
#define P_SIZE 5
#define W_CAPTION "Mapa do Cosmmus"
#define MAX_RESTING_TIME 15000
#define NUM_COLORS 10

#define SWAP(A,B) \
          swapper = A; A = B; B = swapper
#define ABS(X) \
          ((X) > 0)?(X):(-(X))
#define MAX(A,B) \
          A > B ? A : B

using namespace std;

class coord {
  public:
    int X,Y;

    bool operator==(const coord &other) const;      
    bool operator!=(const coord &other) const;    
    coord& operator=(const coord& c);
};

class inflong {
public:
  
  inflong():numcount(0){}
  ~inflong() {}

  double getSum() {
    list<double> summing;
    list<double> summed;
    for (list<long>::iterator it = numbers.begin() ; it != numbers.end() ; it++) {
      summing.push_back((double)(*it));
    }    
    while (summing.size() > 1) {
      summed.clear();
      for (list<double>::iterator it = summing.begin() ; it != summing.end() ; it++) {
        double local_sum = *it;
        if (++it != summing.end())
          local_sum += *it;
        summed.push_back(local_sum);
      }
      summing = summed;
    }
    return summing.front();
  }

  double getAverage() {
    return getSum() / (double)numcount;
  }

  void add(long num) {
    numcount++;
    if (numbers.empty() || LONG_MAX - numbers.front() <= num)
      numbers.push_front(num);
    else {
      long newnum = numbers.front() + num;
      numbers.pop_front();
      numbers.push_front(newnum);
    }
  }

private:
  list<long> numbers;
  long numcount;
};

typedef struct {
  Uint8 R, G, B, A;
} Color;

using namespace std;

void setSdl(SDL_Surface** screen);
Uint32 colorTable(int col_index);
void drawOnScreen(SDL_Surface* screen, int x, int y, unsigned char R, unsigned char G, unsigned char B);
void drawLine(SDL_Surface *screen, int x1, int y1, int x2, int y2, Color linecolor);
void drawLineBresenham(SDL_Surface *screen, int x0, int y0, int x1, int y1, Uint32 linecolor);
void putPixel32(SDL_Surface *surface, int x, int y, Uint32 pixelcolor);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
int approx(float number);
long approxLong(double number);
float apow(float a, int b);
float distance (float xo, float yo, float xf, float yf);
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
SDL_Surface* load_image( string filename );
bool belongsToVisibility (float px, float py, float dx, float dy, float ox, float oy); //visibility is defined as a semicircle with the observer in the center
float convertToScale(float value, float old_min, float old_max, float new_min, float new_max);
float simpleScale(float value, float old_max, float new_max);
string floatToString(float f, int precision = 2);
string longToString(long l);
long absolute(long l);
long minimum(long a, long b);