#pragma once

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

#include <vector>
#include <string>

using namespace std;

#define VIEW_DISTANCE 50
#define THRESHOLD_DISTANCE 10

class Cell;

// class Avatar;//TODO apagar esta linha

class coord;

class Avatar {

  public:

		
    Avatar();    
    ~Avatar();
    void init();
        
    void setDrawable(string my_surface_file, string seen_surface_file, SDL_Surface* out_screen);
    
    void step(unsigned long delay);
					
    void setPlayerId (int i);
    
    void markAsSeen(float relevance_);
		
    float OtherRelevance(Avatar* other);
	
    float GetX();
    float GetY();
		
    void* setImage(string filename);
    
    void draw();
    
    coord getCell ();
    
    void checkCellWeight (Avatar* other);
    
    float getInteraction(Cell* _cell);
    
    static void resetCells ();    
    static void drawCells (SDL_Surface* output);
        
    static void toggleVertex();    
    static void toggleEdge();
		
  protected:

    SDL_Surface* my_surface;
    SDL_Surface* seen_surface;
    float posx, posy;
    int dirx, diry;
    int destx, desty;
    unsigned char R,G,B;
    Uint32 last_move, resting_time;
    bool isSeen;
    float incr_y;
    float incr_x;
    float relevance;
    unsigned long stopped_time;
    bool isDrawable;		
    unsigned long player_id;
    static int m_numofhotspots;
    static vector<coord> mv_hotspotlist;
    static SDL_Surface* surface_vertex_weight;
    static SDL_Surface* surface_edge_weight;
    static float total_weight;
    static bool showv, showe;
    float my_vweight, my_eweight;
    Cell* my_cell;
    SDL_Surface* screen;		
};
