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

class KDTree;

class Server {

  public:

    Server(long power = 0);
    ~Server();

///    int assignRegion (Region* r);
    void releaseRegion();
    static void releaseAllRegions();
///    Region* getRegion();

    void setNode(KDTree* _node);
    KDTree* getNode();

    static Server* getServerById(int _id);
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

///    Region* managedRegion;
    KDTree* treeNode;
    long serverPower;
    int serverId;
    static int lastId;
    static long multiServerPower;
    static list<Server*> serverList;
    static map<int, Server*> serversMap;
    
};
