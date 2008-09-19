#pragma once

#ifdef _WIN32
#include "../../myutils.h"
#else
#include "myutils.h"
#endif

#include <iostream>

using namespace std;

class Region;

class Server {

  public:

    Server();
    Server(float capacity);
    ~Server();

    int assignRegion (Region* r);
    void releaseRegion();
    static void releaseAllRegions();
    Region* getRegion();

    void setServerCapacity(float cap);
    float getServerCapacity();
    static float getMultiserverCapacity();

  protected:

    Region* managedRegion;
    float serverCapacity;
    static float multiServerCapacity;
    static list<Server*> serverList;
    
};
