#pragma once

#ifdef _WIN32
#include "../../myutils.h"
#else
#include "myutils.h"
#endif

#define FAST_SWAP true

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
    list<Cell*> &getCells();
    bool hasCell(Cell* c);  
    Cell* getHeaviestCell();

    void checkNeighborsList();
    static void checkAllRegionsNeighbors();
    list<Region*> &getNeighbors();
    int getNumberOfNeighbors();
    bool hasNeighbor(Region* r);

    bool setServer(Server* s);
    void unsetServer();
    Server* getServer();

    void setRegionCapacity(long cap);
    long getRegionCapacity(void);

    long getRWeight(); //deprecated
    long getRegionWeight();
    long getEWeight(Region* neighbor);
    long getAllEdgesWeight();
    double getWeightFraction();
    double getLoadFraction();
    long getAbsoluteLoad();    		
    static long getWorldLoad();
    static long getEdgeCut();
    double getRegionOverload();

    void updateEWeight(Region* neighbor);
    void updateAllEdges();
    static void updateAllEdgesAllRegions();
       
    void setBorderColor(Uint32 bc);
    void drawRegion(SDL_Surface* output);
    static void drawAllRegions(SDL_Surface* output);
    void drawEdge(SDL_Surface* output, Region* neighbor);
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
    void getProportionalPartition(long weight_to_divide, long free_capacity);
    static void balanceRegions(list<Region*>& regList);
    static void sortRegionsByFreeCapacity();
    static bool compareRegionsFreeCapacity(Region* rA, Region* rB);
    static void sortRegionsByServerPower();
    static void sortRegionsByServerPower(list<Region*> &regionlist);
    static bool compareServerPower(Region* rA, Region* rB);

    // FASE DE REFINAMENTO

    static void swapCellsRegions(Cell* c1, Cell* c2, bool fast=false);
    bool testCellSwap(Cell* loc, Cell* ext, long& gain);
    static void refinePartitioningGlobal(int passes = 0);
    void refinePartitioningLocal(Region* other, int passes = 0);
    static bool refineKL_kwise(list<Region*> &regionsToRefine, int passes = 100);
    static bool refineKL_pairwise(Region* r1, Region* r2);
    static void getBestCellPair(Region* r1, Region* r2, Cell*& c1, Cell*& c2, long* gain = NULL);
    static double getBalancingImprovement(Cell* c1, Cell* c2);

    //REBALANCEAMENTO v1 :: pegar a região mais pesada e ir distribuindo suas células mais leves entre regiões tais que o ganho de balanceamento seja máximo.
    //mas parece haver um problema nessa definição de ganho de balanceamento: é melhor dar a célula a uma região que se aproxime muito do peso ideal do que
    //dar a uma região que tenha muitos recursos. Mas isso pode ser bom, porque primeiro entrega as células mais leves pros servidores mais fracos.

    static void improveBalancing_kwise(list<Region*> &regionsToImproveBalancing, int passes = 100);
    static Region* improveBalancing_pairwise(Region* r1, Region* r2);
    void alleviateOverload(list<Region*> &regionsToImproveBalancing);
    static void sortByOverload(list<Region*> &regionList);
    static bool compareRegionsOverload(Region* rA, Region* rB);
    double getDisbalanceAfterAddingCell(Cell* c);
    double getDisbalanceAfterRemovingCell(Cell* c);
    double getBalancingImprovementForTransfering(Cell* c, Region* r);

    //REBALANCEAMENTO v2

    static void improveBalancing_v2(list<Region*> &regionsToImproveBalancing);
    void alleviateOverload_v2(list<Region*> &regionsToReceive);
    Cell* getCellWithWeightLowerThanButClosestTo(long weight);

    //REBALANCEAMENTO v3

    static void improveBalancing_v3(list<Region*> regionsToImproveBalancing);

    //REBALANCEAMENTO v4

    static void improveBalancing_v4(list<Region*> regionsToImproveBalancing);

    //REBALANCEAMENTO repart (proposta)

    static void improveBalancing_repart(list<Region*> regionsToImproveBalancing);
    void startLocalBalancing();
    Region* getLightestNeighbor(list<Region*> region_list);
    static Region* getHighestCapacityFreeRegion();
    void checkBalancing();

  protected:
    
    Uint32 borderColor;
    list<Cell*> cells;
    list<Region*> neighbors;
    //list<float> edgeWeight;
    map<Region*, long> edgeByRegion;
    Server* parentServer;
    long regionCapacity;
    static long worldCapacity;
    static list<Region*> regionList;
    static bool showr, showe, showw;
    static int numRegions;
};
