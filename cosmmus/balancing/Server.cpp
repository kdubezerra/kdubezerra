#include "Server.h"
#include "Region.h"

//===========================================static members

float Server::multiServerCapacity = 0.0f;
list<Server*> Server::serverList;

//================================cons/des-truction methods

Server::Server(float capacity) : managedRegion(NULL), serverCapacity(capacity) {  
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

void Server::setServerCapacity(float cap) {
  multiServerCapacity -= serverCapacity;
  serverCapacity = cap;
  multiServerCapacity += serverCapacity;
}

float Server::getServerCapacity() {
  return serverCapacity;
}

float Server::getMultiserverCapacity() {
  float totalCap = 0.0f;
  for (list<Server*>::iterator it = serverList.begin() ; it != serverList.end() ; it++) {
    totalCap += (*it)->getServerCapacity();
  }
  return totalCap;
}

bool Server::compareCapacities(Server* sA, Server* sB) {
  return (sA->getServerCapacity() > sB->getServerCapacity());
}

void Server::sortServersByCapacity() {
  serverList.sort(Server::compareCapacities);
}

float Server::getLoad() {
  return getRegion()->getRWeight() / getServerCapacity();
}

bool Server::isDisbalanced() {
  float world_fraction = getRegion()->getRWeight() / Region::getWorldWeight();
  float power_fraction = getServerCapacity() / getMultiserverCapacity();
  return world_fraction / power_fraction > DISBAL_TOLERANCE;
}