/*
 * Group.cpp
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Group.h"

using namespace optpaxos;

Group::Group() {
  groupCoordinator = NULL;
}

Group::~Group() {
  // TODO Auto-generated destructor stub
}

void Group::addServer(Server* _server) {
  serverList.push_back(_server);
}

void Group::removeServer(Server* _server) {
  serverList.remove(_server);
}

std::list<Server*> Group::getServerList() {
  return serverList;
}

void Group::setCoordinator(Server* _server) {
  groupCoordinator = _server;
}

Server* Group::getCoordinator() {
  return groupCoordinator;
}

void Group::addManagedObject(Object* _obj) {
  managedObjects.push_back(_obj);
}

void Group::removeManagedObject(Object* _obj) {
  managedObjects.remove(_obj);
}

std::list<Object*> Group::getGroupObjects() {
  return managedObjects;
}

static std::list<Group*> Group::findGroups(std::string _brokerUrl, unsigned port) {
  std::list<Group*> groupsList;

  return groupsList;
}
