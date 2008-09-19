#pragma once

#ifdef _WIN32
#include "../../myutils.h"
#else
#include "myutils.h"
#endif

#include <iostream>
#include <vector>
#include <list>

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
    void drawWeight(SDL_Surface* output, TTF_Font* font);
    static void drawAllRegionsWeights(SDL_Surface* output, TTF_Font* font);

    static void toggleShowRegions();
    static void toggleShowEdges();
    static void toggleShowRegionWeight();
    
    static int getNumRegions();
    static void divideWorld(int num_reg);
    static void balanceRegions();    
    void getWorldPartitionEXAMPLE();
    void getWorldPartitionRandomStart();
    void getWorldPartition();

  protected:
    
    Uint32 borderColor;
    list<Cell*> cells;
    list<Region*> neighbors;
    list<float> edgeWeight;
    static list<Region*> regionList;
    static bool showr, showe, showw;
    static int numRegions;
};
