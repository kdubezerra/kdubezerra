#pragma once

#include <iostream>
#include <cstdlib>
#include <ctime>

#include <SDL/SDL.h>
#include <SDL.h>
#include <signal.h>

#include <math.h>

#define uint64 unsigned long
#define nplayers WW
#define WW 750 
// ww = world width
#define P_SIZE 5

#define W_CAPTION "Mapa do Cosmmus"

#define MAX_RESTING_TIME 15000

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


#ifdef _SDL_H
void setSdl(SDL_Surface** screen) {
	if( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
		cerr <<	"Couldn't initialize SDL: " << SDL_GetError() << endl;
		exit(1);
	}
	
	
	//poder fechar a janela da SDL
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	atexit(SDL_Quit);

// 	*screen = SDL_SetVideoMode(WW, WW, 8, SDL_SWSURFACE|SDL_ANYFORMAT);
// 	if ( screen == NULL ) {
// 		cerr << "Couldn't set "<< WW << "x" << WW << "x8 video mode: " << SDL_GetError() << endl;
// 		exit(1);
// 	}

// 	SDL_WM_SetCaption(W_CAPTION, 0);

// 	cout << "Set "<< WW << "x" << WW << " at " << (unsigned short) (*screen)->format->BitsPerPixel << " bits-per-pixel mode" << endl;

// 	SDL_FillRect (*screen, 0, SDL_MapRGB((*screen)->format, 0, 0, 0));
// 	SDL_UpdateRect(*screen, 0, 0, 0, 0);
}



void drawOnScreen(SDL_Surface* screen, int x, int y, unsigned char R, unsigned char G, unsigned char B) {
	Uint32 color;
	color = SDL_MapRGB(screen->format, R, G, B);
	static SDL_Rect player_rect;
	
	//x -= P_SIZE/2;
	//y -= P_SIZE/2;
	
	player_rect.w = player_rect.h = P_SIZE;
	player_rect.x = x;
	player_rect.y = y;
	
	
	/* Lock the screen for direct access to the pixels */
	if ( SDL_LockSurface(screen) < 0 ) {
		cerr << "Can’t lock screen: " << SDL_GetError() << endl;
		return;
	}

	//putpixel(screen, x, y, color);
	
	SDL_FillRect (screen, &player_rect, SDL_MapRGB(screen->format, R, G, B));
	//SDL_UpdateRect (screen, x, y, P_SIZE, P_SIZE);
	SDL_UpdateRect (screen, 0, 0, WW, WW);

	//SDL_UpdateRect(screen, x, y, 1, 1);
	//return;

	SDL_UnlockSurface(screen);
	return;

	/* Update just the part of the display that we’ve changed */
}



void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	switch(bpp) {
		case 1:
			*p = pixel;
			break;
		case 2:
			*(Uint16 *)p = pixel;
			break;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			} else {
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
}



SDL_Surface* load_image( string filename ) 
{ 
	SDL_Surface* loadedImage = NULL; 
	SDL_Surface* optimizedImage = NULL;    
	loadedImage = SDL_LoadBMP( filename.c_str() );    
    
	if( loadedImage != NULL )
	{        
		optimizedImage = SDL_DisplayFormat( loadedImage );
		SDL_FreeSurface( loadedImage );
	}    
    
	return optimizedImage;
}



void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{    
	SDL_Rect offset;    
	
	offset.x = x;
	offset.y = y;    
   
	SDL_BlitSurface( source, NULL, destination, &offset );
}

#endif

int approx(float number) {
	float round_up_diff = ceil(number) - number;
	float round_dn_diff = number - floor(number);
	
	return round_up_diff > round_dn_diff ? (int)floor(number) : (int)ceil(number);	
}

float distance (float xo, float yo, float xf, float yf) {
	return sqrt( apow(xf-xo,2) + apow(yf-yo,2) );
}

float apow(float a, int b) { //power ... only works with non-negative exponnents (b)
	if (b == 0) return 1;
	return a * apow(a, b - 1);
}

bool belongsToVisibility (float px, float py, float dx, float dy, float ox, float oy) {

	float hipo = distance (0.0, 0.0, dx, dy);
	float cos_alpha = dx / hipo;
	float sin_alpha = dy / hipo;
// 	float oy__, py__;
	float ox__, px__;

	ox__ = ox * cos_alpha + oy * sin_alpha;
	px__ = px * cos_alpha + py * sin_alpha;

// 	oy__ = -ox * sin_alpha + oy * cos_alpha;
// 	py__ = -px * sin_alpha + py * cos_alpha;

	if (ox__ >= px__)
		return true;
	else
		return false;

}
