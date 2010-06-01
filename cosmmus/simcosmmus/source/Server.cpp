#include "../headers/Server.h"
#include "../headers/Region.h"
#include "../headers/Cell.h"
#include "../headers/KDTree.h"
#include "../headers/Simulation.h"

//===========================================static members

long Server::multiServerPower = 0;
list<Server*> Server::serverList;
map<int, Server*> Server::serversMap;
int Server::lastId = 0;;

//================================cons/des-truction methods

Server::Server(long power) : managedRegion(NULL), serverPower(power) {  
  serverId = lastId++;
  serversMap[serverId] = this;
  serverList.push_back(this);
  treeNode = NULL;
  overHead = 0;
}

Server::~Server() {
  list<Server*>::iterator it;
  serversMap.erase(serverId);
  for (it = serverList.begin() ; it != serverList.end() && *it != this ; it++);
  if (it != serverList.end())
    serverList.erase(it);
}    

//============================================other methods

int Server::assignRegion(Region* r) {
  if (managedRegion || !r) return 0;
  managedRegion = r;
  r->setServer(this);
  r->setRegionCapacity(getServerPower());
  return 1;
}

void Server::releaseRegion() {
  if (!managedRegion) return;
  Region* r = managedRegion;
  managedRegion = NULL;
  r->unsetServer();  
}

void Server::releaseAllRegions() {
  for (list<Server*>::iterator it = serverList.begin() ; it != serverList.end() ; it++) {
    (*it)->releaseRegion();
  }
}

Region* Server::getRegion() {
  return managedRegion;
}

void Server::setNode(KDTree *_node) {
  treeNode = _node;
}

KDTree* Server::getNode() {
  return treeNode;
}

Server* Server::getServerById(int _id) {
  return serversMap[_id];
}

void Server::setServerPower(long pow) {
  multiServerPower -= serverPower;
  serverPower = pow;
  multiServerPower += serverPower;
}

long Server::getServerPower() {
  return serverPower;
}

long Server::getMultiserverPower() {
  long totalPow = 0;
  for (list<Server*>::iterator it = serverList.begin() ; it != serverList.end() ; it++) {
    totalPow += (*it)->getServerPower();
  }
  return totalPow;
}

bool Server::comparePower(Server* sA, Server* sB) {
  return (sA->getServerPower() > sB->getServerPower());
}

void Server::sortServersByPower() {
  serverList.sort(Server::comparePower);
}

long Server::getLoad() {
  return getRegion()->getAbsoluteLoad() / getServerPower();
}

bool Server::isDisbalanced() {
  double world_fraction = getRegion()->getAbsoluteLoad() / Region::getWorldLoad();
  double power_fraction = getServerPower() / getMultiserverPower();
  return world_fraction / power_fraction > DISBAL_TOLERANCE;
}

double Server::getPowerFraction() {
  return (double)getServerPower() / (double)getMultiserverPower();
}

long Server::getWeight() {
  if (Simulation::getSpacePartMethod() == KDTREE) {
    if (this->treeNode)
      return this->treeNode->getWeight();
    else
      return 0;
  }

  else if (Simulation::getSpacePartMethod() == CELLS) {
    if (this->getRegion())
      return this->getRegion()->getRegionWeight();
    else
      return 0;
  }
}

double Server::getUsage() {
  return (double) this->getWeight() / (double) this->getServerPower();
}

double Server::getUsageDeviation() {
  double global_usage;
  if (Simulation::getSpacePartMethod() == KDTREE) {
    global_usage = (double)(KDTree::getRoot()->getWeight()) / (double)Server::getMultiserverPower();
  }
  else if (Simulation::getSpacePartMethod() == CELLS) {
    global_usage = (double)(Cell::getWorldWeight()) / (double)Server::getMultiserverPower();
  }
  double sum_of_square_diffs = 0.0f;
  for (list<Server*>::iterator it = serverList.begin() ; it != serverList.end() ; it++) {
    sum_of_square_diffs += pow((*it)->getUsage() - global_usage, 2);
  }
  double mean_sqr_diff = sum_of_square_diffs / (double) serverList.size();
  return sqrt(mean_sqr_diff);
}

void Server::clearOverhead() {
  for (list<Server*>::iterator it = serverList.begin() ; it != serverList.end() ; it++) {
    (*it)->overHead = 0;
  }
}

long Server::getOverhead() {
  if (Simulation::getSpacePartMethod() == KDTREE) {
    return overHead;
  }
  else if (Simulation::getSpacePartMethod() == CELLS) {
    if (this->getRegion())
      return this->getRegion()->getNeighborsOverhead();
    else
      return 0;
  }
}

void Server::incOverhead(long value) {
  overHead += value;
}