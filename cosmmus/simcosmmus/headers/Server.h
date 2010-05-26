#pragma once

#include "../headers/myutils.h"

#include <algorithm>
#include <iostream>
#include <list>

#define DISBAL_TOLERANCE 1.2f

using namespace std;

class Region;

class Server {

  public:

    Server(long power = 0);
    ~Server();

    int assignRegion (Region* r);
    void releaseRegion();
    static void releaseAllRegions();
    Region* getRegion();

    void setServerPower(long pow);
    long getServerPower();
    static long getMultiserverPower();
    
    static bool comparePower(Server* sA, Server* sB);
    static void sortServersByPower();
    long getLoad();
    bool isDisbalanced();
    double getPowerFraction();
    
    long getWeight();
    double getUsage();
    static double getUsageDeviation();
    long getOverhead();

  protected:

    Region* managedRegion;
    long serverPower;
    static long multiServerPower;
    static list<Server*> serverList;
    
};
