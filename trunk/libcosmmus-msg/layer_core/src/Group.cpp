/*
 * Group.cpp
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>

#include "../include/Group.h"
#include "../include/NodeInfo.h"
#include "../include/ObjectInfo.h"

using namespace std;
using namespace optpaxos;
using namespace netwrapper;

std::list<Group*> Group::groupList;

Group::Group() {
  id = -1;
  groupCoordinator = NULL;
}

Group::Group(int _id) {
  id = _id;
  groupCoordinator = NULL;
}

Group::Group(Group* _other) {
  id = _other->id;
  groupCoordinator = new NodeInfo(_other->groupCoordinator);
  for (std::list<NodeInfo*>::iterator it = _other->serverList.begin() ; it != _other->serverList.end() ; it++) {
    serverList.push_back(new NodeInfo(*it));
  }
  for (std::map<int, ObjectInfo*>::iterator it = _other->managedObjects.begin() ; it != _other->managedObjects.end() ; it++) {
    managedObjects[it->first] = new ObjectInfo(it->second);
  }
}

Group::~Group() {
  if (groupCoordinator != NULL)
    delete groupCoordinator;
  for (std::list<NodeInfo*>::iterator it = serverList.begin() ; it != serverList.end() ; it++) {
    delete *it;
  }
  for (std::map<int, ObjectInfo*>::iterator it = managedObjects.begin() ; it != managedObjects.end() ; it++) {
    delete it->second;
  }
}

void Group::setId(int _id) {
  id = _id;
}

int Group::getId() {
  return id;
}

void Group::addServer(NodeInfo* _server) {
  serverList.push_back(new NodeInfo(_server));
}

void Group::removeServer(NodeInfo* _server) {
  for (std::list<NodeInfo*>::iterator it = serverList.begin() ; it != serverList.end() ; it++)
    if ((*it)->getNodeId() == _server->getNodeId()) {
      delete *it;
      serverList.erase(it);
      break;
    }
}

std::list<NodeInfo*> Group::getServerList() {
  return serverList;
}

void Group::setCoordinator(NodeInfo* _server) {
  if (groupCoordinator != NULL)
    delete groupCoordinator;
  groupCoordinator = new NodeInfo(_server);
}

NodeInfo* Group::getCoordinator() {
  return groupCoordinator;
}

void Group::addManagedObject(ObjectInfo* _obj) {
  managedObjects[_obj->getId()] = new ObjectInfo(_obj);
}

bool Group::hasObject(ObjectInfo* _obj) {
  return managedObjects.find(_obj->getId()) != managedObjects.end();
}

void Group::removeManagedObject(ObjectInfo* _obj) {
  std::map<int, ObjectInfo*>::iterator finder = managedObjects.find(_obj->getId());
  if (finder == managedObjects.end())
    return;
  delete finder->second;
  managedObjects.erase(_obj->getId());
}

std::map<int, ObjectInfo*> Group::getObjectsIndex() {
  return managedObjects;
}

std::list<ObjectInfo*> Group::getObjectsList() {
  std::list<ObjectInfo*> objList;
  for (std::map<int, ObjectInfo*>::iterator it = managedObjects.begin() ; it != managedObjects.end() ; it++) {
    objList.push_back(it->second);
  }
  return objList;
}

void Group::addGroup(Group* _grp) {
  groupList.push_back(_grp);
}

std::list<Group*> Group::requestGroupsList(std::string _brokerUrl, unsigned port) {
  std::list<Group*> groupsList;
  // TODO: ... everything
  return groupsList;
}

std::list<Group*> Group::getGroupsList() {
  return groupList;
}

Message* Group::packToNetwork(Group* _group) {
  Message* grpMsg = new Message();
  grpMsg->addInt(_group->getId());
  grpMsg->addMessage(NodeInfo::packListToNetwork(_group->serverList));
  grpMsg->addMessage(NodeInfo::packToNetwork(_group->groupCoordinator));
  grpMsg->addMessage(ObjectInfo::packIndexToNetwork(_group->managedObjects));
  return grpMsg;
}

Message* Group::packListToNetwork(std::list<Group*> _groupList) {
  Message* grpListMsg = new Message();
  grpListMsg->addInt((int) _groupList.size());
  for (std::list<Group*>::iterator it = _groupList.begin() ; it != _groupList.end() ; it++) {
    grpListMsg->addMessage(Group::packToNetwork(*it));
  }
  return grpListMsg;
}

Group* Group::unpackFromNetwork(Message* _msg) {
  Group* grp = new Group();
  grp->id = _msg->getInt(0);
  grp->serverList = NodeInfo::unpackListFromNetwork(_msg->getMessage(0));
  grp->groupCoordinator = NodeInfo::unpackFromNetwork(_msg->getMessage(1));
  grp->managedObjects = ObjectInfo::unpackIndexFromNetwork(_msg->getMessage(2));
  return grp;
}

std::list<Group*> Group::unpackListFromNetwork(Message* _msg) {
  std::list<Group*> grpList;
  int count = _msg->getInt(0);
  for (int i = 0 ; i < count ; i++) {
    grpList.push_back(Group::unpackFromNetwork(_msg->getMessage(i)));
  }
  return grpList;
}
