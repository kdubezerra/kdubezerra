#pragma once

#include "../headers/myutils.h"

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
    long getNeighborsOverhead();
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

    static void setMethod(short method);
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

    //REBALANCEAMENTO v1 :: pegar a regi�o mais pesada e ir distribuindo suas c�lulas mais leves entre regi�es tais que o ganho de balanceamento seja m�ximo.
    //mas parece haver um problema nessa defini��o de ganho de balanceamento: � melhor dar a c�lula a uma regi�o que se aproxime muito do peso ideal do que
    //dar a uma regi�o que tenha muitos recursos. Mas isso pode ser bom, porque primeiro entrega as c�lulas mais leves pros servidores mais fracos.

    static void improveBalancing_kwise(list<Region*> &regionsToImproveBalancing, int passes = 100);
    static Region* improveBalancing_pairwise(Region* r1, Region* r2);
    void alleviateOverload(list<Region*> &regionsToImproveBalancing);
    static void sortByOverload(list<Region*> &regionList);
    static bool compareRegionsOverload(Region* rA, Region* rB);
    double getDisbalanceAfterAddingCell(Cell* c);
    double getDisbalanceAfterRemovingCell(Cell* c);
    double getBalancingImprovementForTransfering(Cell* c, Region* r);

    //REBALANCEAMENTO v2 - bfbct

    static void rebalance_bfbct(list<Region*> regionsToRebalance);
    void alleviateOverload_v2(list<Region*> &regionsToReceive);
    Cell* getCellWithWeightLowerThanButClosestTo(long weight);

    //REBALANCEAMENTO v3 - progrega-kh

    static void rebalance_progrega_kh(list<Region*> regionsToRebalance);

    //REBALANCEAMENTO v4 - progrega-kf

    static void rebalance_progrega_kf(list<Region*> regionsToRebalance);

    //REBALANCEAMENTO repart (proposta) - progrega

    static void rebalance_progrega(list<Region*> regionsToRebalance);
    void startLocalBalancing();
    Region* getLightestNeighbor(list<Region*> region_list);
    static Region* getHighestCapacityFreeRegion();
    void checkBalancing();
    
    //REBALANCEAMENTO Ahmed and Shirmohammadi - AS
    
    void rebalance_as();
    static Region* getLightestRegion();
    static Region* getLeastOverloadableRegion(Cell* c);
    list<Cell*> getSmallestCluster();
    Cell* getLessInteractingCell(list<Cell*> cluster);
    

  protected:
    
    Uint32 borderColor;
    list<Cell*> cells;
    list<Region*> neighbors;
    //list<float> edgeWeight;
    map<Region*, long> edgeByRegion;
    Server* parentServer;
    long regionCapacity;
    static short rebalMethod;
    static long worldCapacity;
    static list<Region*> regionList;
    static bool showr, showe, showw;
    static int numRegions;
};

