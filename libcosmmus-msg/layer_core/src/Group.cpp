/*
 * Group.cpp
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Group.h"
#include "../include/NodeInfo.h"
#include "../include/ObjectInfo.h"

using namespace optpaxos;

Group::Group() {
  groupCoordinator = NULL;
}

Group::Group(Group* _group) {
  groupCoordinator = new NodeInfo(_group->groupCoordinator);
  for (std::list<ObjectInfo*>::iterator it = _group->managedObjects.begin() ; it != _group->managedObjects.end() ; it++) {
    managedObjects.push_back(new ObjectInfo(*it));
  }
}

Group::~Group() {
  // TODO Auto-generated destructor stub
}

void Group::addServer(NodeInfo* _server) {
  serverList.push_back(_server);
}

void Group::removeServer(NodeInfo* _server) {
  serverList.remove(_server);
}

std::list<NodeInfo*> Group::getServerList() {
  return serverList;
}

void Group::setCoordinator(NodeInfo* _server) {
  groupCoordinator = _server;
}

NodeInfo* Group::getCoordinator() {
  return groupCoordinator;
}

void Group::addManagedObject(ObjectInfo* _obj) {
  managedObjects.push_back(_obj);
}

void Group::removeManagedObject(ObjectInfo* _obj) {
  managedObjects.remove(_obj);
}

std::list<ObjectInfo*> Group::getGroupObjects() {
  return managedObjects;
}

std::list<Group*> Group::findGroups(std::string _brokerUrl, unsigned port) {
  std::list<Group*> groupsList;

  return groupsList;
}
