#pragma once

#include <iostream>
#include <list>

using namespace std;

class Cell;

class Region {

  public:

    Region();

    ~Region();

    void subscribe(Cell* c);
    void unsubscribe(Cell* c);

    list<Cell*> &getCells();
    list<Region*> &getNeighbors();
    int getNumberOfNeighbors();

    float getRWeight();
    float getEWeight(int neighbor);
    int updateEWeight(int neighbor);
    void updateAllEdges();

    Region* getNeighbor(int neighbor);

    void drawRegion(SDL_Surface* output);
    static void drawAllRegions(SDL_Surface* output);
    void drawEdge(int neighbor, SDL_Surface* output);
    static void toggleShowRegions();
    static void toggleShowEdges();
    
    static void divideWorld(int num_reg);
    static void balanceRegions();

  protected:
    
    list<float> edgeWeight;
    list<Cell*> cells;
    list<Region*> neighbors;
    static list<Region*> regionList;
    static bool showr, showe;
};
