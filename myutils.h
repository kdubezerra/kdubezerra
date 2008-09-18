#pragma once

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

#include <iostream>
#include <cstdlib>
#include <ctime>
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

class coord {
  public:
    int X,Y;

    bool operator==(const coord &other) const;      
    bool operator!=(const coord &other) const;    
    coord& operator=(const coord& c);
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
float apow(float a, int b);
float distance (float xo, float yo, float xf, float yf);
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
SDL_Surface* load_image( string filename );
bool belongsToVisibility (float px, float py, float dx, float dy, float ox, float oy); //visibility is defined as a semicircle with the observer in the center
float convertToScale(float value, float old_min, float old_max, float new_min, float new_max);
float simpleScale(float value, float old_max, float new_max);