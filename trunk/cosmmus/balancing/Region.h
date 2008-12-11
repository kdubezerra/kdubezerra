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
class Server;

class Region {

  public:

    Region(Uint32 borderColor = 0x000000);
    ~Region();

    void subscribe(Cell* c);
    void unsubscribe(Cell* c);
    void unsubscribeAllCells();
    
    bool setServer(Server* s);
    void unsetServer();
    Server* getServer();

    list<Cell*> &getCells();
    list<Region*> &getNeighbors();
    int getNumberOfNeighbors();
    Region* getNeighbor(int neighbor);
    bool hasCell(Cell* c);

    void setRegionCapacity(float cap);
    float getRegionCapacity(void);

    float getRWeight();
    float getEWeight(int neighbor);
    float getAllEdgesWeight();
    float getAbsoluteLoad();
    float getLoadFraction();
    static float getWorldLoad();
    static float getEdgeCut();

    void updateEWeight(int neighbor);
    void updateAllEdges();    
       
    void setBorderColor(Uint32 bc);
    void drawRegion(SDL_Surface* output);
    static void drawAllRegions(SDL_Surface* output);
    void drawEdge(SDL_Surface* output, int neighbor);
    void drawAllEdges(SDL_Surface* output);
    static void drawAllRegionsEdges(SDL_Surface* output);
    void drawLoad(SDL_Surface* output, TTF_Font* font);
    static void drawAllRegionsWeights(SDL_Surface* output, TTF_Font* font);

    static void toggleShowRegions();
    static void toggleShowEdges();
    static void toggleShowRegionWeight();
    
    static list<Region*> &getRegionList();
    static int getNumRegions();
    
    // FASE DE PARTICIONAMENTO (DEPRECATED)
    
    static void balanceRegions();    
    void getWorldPartitionEXAMPLE();
    void getWorldPartitionRandomStart();
    void getWorldPartition();

    // FASE DE PARTICIONAMENTO (WORKING)

    static void initRegions(int num_reg);
    static void disposeRegions();
    static void partitionWorld();
    static void distributeOrphanCells();
    static void balanceWorld();
    void getProportionalPartition();
    static void balanceRegions(list<Region*>& regList);
    static void sortRegionsByFreeCapacity();
    static bool compareRegionsFreeCapacity(Region* rA, Region* rB);
    static void sortRegionsByServerPower();
    static bool compareServerPower(Region* rA, Region* rB);

    // FASE DE REFINAMENTO

    static void swapCellsRegions(Cell* c1, Cell* c2);
    bool testCellSwap(Cell* loc, Cell* ext, float &gain);
    static void refinePartitioningGlobal(int passes = 0);
    void refinePartitioningLocal(Region* other, int passes = 0);
    static void getBestCellPair(Region* r1, Region* r2, Cell*& c1, Cell*& c2, float* gain = NULL);    

  protected:
    
    Uint32 borderColor;
    list<Cell*> cells;
    list<Region*> neighbors;
    list<float> edgeWeight;
    Server* parentServer;
    float regionCapacity;
    static float worldCapacity;
    static list<Region*> regionList;
    static bool showr, showe, showw;
    static int numRegions;
};
