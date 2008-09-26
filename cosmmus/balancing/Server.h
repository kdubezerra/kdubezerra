#pragma once

#ifdef _WIN32
#include "../../myutils.h"
#else
#include "myutils.h"
#endif

#include <algorithm>
#include <iostream>
#include <list>

#define DISBAL_TOLERANCE 1.2f

using namespace std;

class Region;

class Server {

  public:

    Server(float power = 0.0f);
    ~Server();

    int assignRegion (Region* r);
    void releaseRegion();
    static void releaseAllRegions();
    Region* getRegion();

    void setServerPower(float pow);
    float getServerPower();
    static float getMultiserverPower();
    
    static bool comparePower(Server* sA, Server* sB);
    static void sortServersByPower();
    float getLoad();
    bool isDisbalanced();

  protected:

    Region* managedRegion;
    float serverPower;
    static float multiServerPower;
    static list<Server*> serverList;
    
};
