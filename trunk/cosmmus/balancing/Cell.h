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

//edge-drawing calculations
//EDGE_POS_MIDDLE = x + CELL_LENGTH/2 - EDGE_SIZE/2
//EDGE_POS_CORNER = x + CELL_LENGTH/2 - EDGE_SIZE
#define EDGE_SIZE 6
#define EDGE_POS_MIDDLE 22
#define EDGE_POS_CORNER 44

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
    void updateAllEdges();
    
    static Cell* getCell(int cell_X, int cell_Y);
    Cell* getNeighbor(short neigh);
    
    static int getRowLength();
    static void allocCellMatrix(int cells_on_a_row);
    static void drawCells(SDL_Surface* output);
    void drawEdge(short neighbor, SDL_Surface* output);
    static void toggleShowVertexWeight();
    static void toggleShowEdgeWeight();
    static void setCellSurfaces (string vertex_weight_imgfile, string edge_weight_imgfile);    
    
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
