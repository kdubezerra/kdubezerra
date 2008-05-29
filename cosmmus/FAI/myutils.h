#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>

#include <SDL/SDL.h>
// #include <SDL.h>
#include <signal.h>

#include <math.h>

#define uint64 unsigned long
#define nplayers 100
#define WW 300
// ww = world width
#define P_SIZE 5

#define W_CAPTION "Mapa do Cosmmus"

#define MAX_RESTING_TIME 15000

/**
   rever esses tempos posteriormente. tem que ser simulável (serão x * 3 simulações).
	x = numero de pontos do eixo x (jogadores). provavelmente x = 8, mas deveria ser menos...(?)
	3 simulações => c/s, p2pse s/ aoi, p2pse c/ aoi
 **/

#define MAX_SOCIAL_TIME 600000
#define MAX_ACTION_TIME 600000

using namespace std;

void setSdl(SDL_Surface** screen);
void drawOnScreen(SDL_Surface* screen, int x, int y, unsigned char R, unsigned char G, unsigned char B);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
int approx(float number);
float apow(float a, int b);
float distance (float xo, float yo, float xf, float yf);
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination );
SDL_Surface* load_image( string filename );
bool belongsToVisibility (float px, float py, float dx, float dy, float ox, float oy); //visibility is defined as a semicircle with the observer in the center
