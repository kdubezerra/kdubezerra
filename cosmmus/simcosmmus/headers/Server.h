#pragma once

#include "../headers/myutils.h"

#include <algorithm>
#include <iostream>
#include <list>

#define DISBAL_TOLERANCE 1.2f

using namespace std;

class Region;
class KDTree;
class BSPTree;

class Server {

  public:

    Server(long power = 0);
    ~Server();

    int assignRegion (Region* r);
    void releaseRegion();
    static void releaseAllRegions();
    Region* getRegion();
    void setNode(KDTree* _node);
    void setNode(BSPTree* _node);
    KDTree* getNode();

    static Server* getServerById(int _id);
    void setServerPower(long pow);
    long getServerPower();
    static long getMultiserverPower();
    static list<Server*> getServerList();
    
    static bool comparePower(Server* sA, Server* sB);
    static void sortServersByPower();
    long getLoad();
    bool isDisbalanced();
    double getPowerFraction();
    
    long getWeight();
    double getUsage();
    static double getUsageDeviation();
    
    static void clearOverhead();
    long getOverhead();
    void incOverhead(long value);

  protected:

    Region* managedRegion;
    KDTree* treeNode;
    BSPTree* bsptreeNode;
    long serverPower;
    int serverId;
    long overHead;
    static int lastId;
    static long multiServerPower;
    static list<Server*> serverList;
    static map<int, Server*> serversMap;
    
};
