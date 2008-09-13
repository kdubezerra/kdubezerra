#pragma once

#include <list>
#include <string>
#include <SDL/SDL.h>

#define NUM_NEIGH 8

//NEIGHBORS
#define UP 0
#define UP_RIGHT 1
#define DOWN_RIGHT 2
#define DOWN 3
#define DOWN_LEFT 4
#define UP_LEFT 5
//when using hexagons, or shifted squares, there is no need for right and left neighbors
#define RIGHT 6
#define LEFT 7

//parameters for the cells
#define CELLS_ON_A_ROW 15
#define CELL_LENGTH int(WW/CELLS_ON_A_ROW)

using namespace std;

class Avatar;

class coord;

class Cell {
    
  public:
    
    Cell();    
    Cell(int coord_x, int coord_y);
    
    ~Cell();
    
    void subscribe(Avatar* av);
    
    void unsubscribe(Avatar* av);
    
    list<Avatar*> &getAvatars();
    
    float getVWeight();
    float getEWeight(short neighbor);
    int updateEWeight(short neighbor);
    void udpdateAllEdges();
    
    static Cell* getCell(int cell_X, int cell_Y);
    Cell* getNeighbor(short neigh);
    
    static void allocCellMatrix(int cells_on_a_row);
    static void drawCells(SDL_Surface* output);
    static void toggleShowVertexWeight();
    static void toggleShowEdgeWeight();
    static void setCellSurfaces (string vertex_weight_imgfile, string edge_weight_imgfile);
    static int getRowLength();
    
  protected:
    
    coord* cellposition;
    float edgeWeight[NUM_NEIGH];
    list<Avatar*> avatars;
    static Cell*** cellMatrix;
    static int cells_on_a_row;
    static bool showv, showe;
    static SDL_Surface* surface_vertex_weight;
    static SDL_Surface* surface_edge_weight;
};