#include "Server.h"
#include "Region.h"

//===========================================static members

long Server::multiServerPower = 0;
list<Server*> Server::serverList;

//================================cons/des-truction methods

Server::Server(long power) : managedRegion(NULL), serverPower(power) {  
  serverList.push_back(this);
}

Server::~Server() {
  list<Server*>::iterator it;
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