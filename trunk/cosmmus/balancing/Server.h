#pragma once

#ifdef _WIN32
#include "../../myutils.h"
#else
#include "myutils.h"
#endif

#include <algorithm>
#include <iostream>
#include <list>

using namespace std;

class Region;

class Server {

  public:

    Server(float capacity = 0.0f);
    ~Server();

    int assignRegion (Region* r);
    void releaseRegion();
    static void releaseAllRegions();
    Region* getRegion();

    void setServerCapacity(float cap);
    float getServerCapacity();
    static float getMultiserverCapacity();
    
    static bool compareCapacities(Server* sA, Server* sB);
    static void sortServersByCapacity();

  protected:

    Region* managedRegion;
    float serverCapacity;
    static float multiServerCapacity;
    static list<Server*> serverList;
    
};
