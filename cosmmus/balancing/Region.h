#pragma once

#include <iostream>
#include <vector>
#include <list>
#include "myutils.h"

using namespace std;

class Cell;

class Region {

  public:

    Region();
    Region(Uint32 borderColor);
    ~Region();

    void subscribe(Cell* c);
    void unsubscribe(Cell* c);
    void unsubscribeAllCells();

    list<Cell*> &getCells();
    list<Region*> &getNeighbors();
    int getNumberOfNeighbors();
    bool hasCell(Cell* c);

    float getRWeight();
    float getEWeight(int neighbor);
    void updateEWeight(int neighbor);
    void updateAllEdges();

    Region* getNeighbor(int neighbor);
       
    void setBorderColor(Uint32 bc);
    void drawRegion(SDL_Surface* output);
    static void drawAllRegions(SDL_Surface* output);
    void drawEdge(SDL_Surface* output, int neighbor);
    void drawAllEdges(SDL_Surface* output);
    static void drawAllRegionsEdges(SDL_Surface* output);
    static void toggleShowRegions();
    static void toggleShowEdges();
    
    static int getNumRegions();
    static void divideWorld(int num_reg);
    static void balanceRegions();
    void getWorldPartition();
    void getWorldPartitionEXAMPLE();

  protected:
    
    Uint32 borderColor;
    list<Cell*> cells;
    list<Region*> neighbors;
    list<float> edgeWeight;
    static list<Region*> regionList;
    static bool showr, showe;
    static int numRegions;
};
