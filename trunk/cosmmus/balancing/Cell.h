#pragma once

#ifdef _WIN32
#include <SDL.h>
#include "../../myutils.h"
#else
#include <SDL/SDL.h>
#include "myutils.h"
#endif

#include <list>
#include <string>

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
// #define CELL_LENGTH int(WW/CELLS_ON_A_ROW)
#define CELL_LENGTH 50

//edge-drawing calculations
//EDGE_POS_MIDDLE = x + CELL_LENGTH/2 - EDGE_SIZE/2
//EDGE_POS_CORNER = x + CELL_LENGTH/2 - EDGE_SIZE
#define EDGE_SIZE 6
#define EDGE_POS_MIDDLE 22
#define EDGE_POS_CORNER 44

using namespace std;

class Region;

class Avatar;

class coord;

// typedef struct Color;

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
    float getEWeight(Cell* neighbor);
    static float getTotalWeight();
    
    void updateVWeight();
    int updateEWeight(short neighbor);
    void updateAllEdges();
    static void updateAllEdgesAndVertexWeights();
    
    static Cell* getCell(int cell_X, int cell_Y);
    Cell* getNeighbor(short neigh);
    short getNeighbor(Cell* neigh);
    Cell* getHighestEdgeFreeNeighbor();
    list<Cell*> getAllNeighbors();
    static Cell* getHighestEdgeFreeNeighbor(list<Cell*> &cellList);
    
    static list<Cell*> getAllCells(bool mustBeFree=false);    
    static Cell* getHeaviestCell(bool mustBeFree=false);
    static list<Cell*> getAllFreeCells();
    static Cell* getHeaviestFreeCell();

    Region* getParentRegion();
    void setParentRegion(Region* region);
    void releaseCellFromRegion();    
    
    coord getCellMatrixPosition();
    coord getAbsolutePosition();
    static int getCellLength();
    static int getRowLength();
    static int getNumNeigh();
    
    static void allocCellMatrix(int cells_on_a_row);
    static void drawCells(SDL_Surface* output);
    void drawEdge(short neighbor, SDL_Surface* output);    
    void drawBorder(SDL_Surface* output, short border, Uint32 bordercolor);
    void drawAllBorders(SDL_Surface* output, Uint32 bordercolor);
    static void toggleShowVertexWeight();
    static void toggleShowEdgeWeight();
    static void setCellSurfaces (string vertex_weight_imgfile, string edge_weight_imgfile);    
    
  protected:
    
    Region* parentRegion;
    coord* cellposition;
    float vertexWeight;
    float edgeWeight[NUM_NEIGH];
    list<Avatar*> avatars;
    static Cell*** cellMatrix;
    static int cells_on_a_row;
    static float totalWeight;
    static bool showv, showe;
    static SDL_Surface* surface_vertex_weight;
    static SDL_Surface* surface_edge_weight;
};
