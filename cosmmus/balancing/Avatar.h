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
class Region;

// class Avatar;//TODO apagar esta linha

class coord;

class Avatar {

  public:

		
    Avatar();    
    ~Avatar();
    void init();
        
    void setDrawable(string my_surface_file, string seen_surface_file, SDL_Surface* out_screen);
    
    void step(unsigned long delay);

    void checkMigration();
    static long getMigrationStill(bool clear_migs = true);
    static long getMigrationWalk(bool clear_migw = true);
					
    void setPlayerId (int i);
    
    void markAsSeen(int relevance_);
		
    int OtherRelevance(Avatar* other);
	
    float GetX();
    float GetY();
		
    void* setImage(string filename);
    
    void draw();
    
    coord getCell ();
    
    void checkCellWeight (Avatar* other);
    
    long getInteraction(Cell* _cell);
    long getWeight();
    long getWeightBruteForce();
    
    static void resetCells ();    
    static void drawCells (SDL_Surface* output);
        
    static bool toggleVertex();
    static bool toggleEdge();
    static bool toggleMobility();
    static bool setMobility(bool value);
		
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
    int relevance;
    unsigned long stopped_time;
    bool isDrawable;		
    unsigned long player_id;
    static int m_numofhotspots;
    static vector<coord> mv_hotspotlist;
    static SDL_Surface* surface_vertex_weight;
    static SDL_Surface* surface_edge_weight;
    static long total_weight;
    static bool showv, showe, isMobile;
    float my_vweight, my_eweight;
    Cell* my_cell;
    Cell* old_cell;
    Region* old_region;
    SDL_Surface* screen;
    static long migration_walk;
    static long migration_still;
    static Avatar* first;
    static list<Avatar*> avList;
};
