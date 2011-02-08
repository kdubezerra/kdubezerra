/*
 * Group.cpp
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Group.h"

Group::Group() {
  groupManager = NULL;
}

Group::~Group() {
  // TODO Auto-generated destructor stub
}

void Group::addServer(GameServer* _server) {
  serverList.push_back(_server);
}

void Group::removeServer(GameServer* _server) {
  serverList.remove(_server);
}

std::list<GameServer*> Group::getServerList() {
  return serverList;
}

void Group::setManager(GameServer* _server) {
  groupManager = _server;
}

GameServer* Group::getManager() {
  return groupManager;
}

void Group::addManagedObject(GameObject* _obj) {
  managedObjects.push_back(_obj);
}

void Group::removeManagedObject(GameObject* _obj) {
  managedObjects.remove(_obj);
}

std::list<GameObject*> Group::getGroupObjects() {
  return managedObjects;
}
