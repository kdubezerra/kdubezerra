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
    
    if (!numcount) return 0.0f;
    
    long max = numbers.size();
    double return_value;
    double* sum_array = new double[max];

    long i = 0;

    for (list<long>::iterator it = numbers.begin() ; it != numbers.end() ; it++) {
      sum_array[i++] = (double) (*it);
    }
    
    while (max > 1) {
      ///se o último elemento de índice par não tiver sucessor para somar,
      ///ou seja, o vetor tem um número ímpar de elementos,
      ///nesse caso, some o último elemento ao menor dos outros elementos presentes no array
      ///e desconsidere o último elemento, passando a ter um número par de elementos no array
      if (max%2) {
        int smallest = 0;
        for (int j = 0 ; j < max-1 ; j++) {
          if (sum_array[j] < sum_array[smallest]) smallest = j;
        }
        sum_array[smallest] += sum_array[max - 1];
        max--;
      }
      for (i = 0 ; i < max ; i+=2 ) {
        sum_array[i/2] = sum_array[i] + sum_array[i+1];
      }
      max /= 2;
    }
    
    return_value = sum_array[0];
    delete [] sum_array;
    return return_value;
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
int intPow(int a, int b);
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
