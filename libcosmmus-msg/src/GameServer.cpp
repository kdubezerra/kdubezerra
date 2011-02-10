/*
 * GameServer.cpp
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/GameServer.h"
#include "../include/Object.h"
#include "../include/Player.h"

GameServer::GameServer() {
  objectModel = NULL;
  serverGroup = NULL;
  coreServer = new Server();
  coreServer->setCallbackServerInterface(this);
}

GameServer::~GameServer() {
  // TODO Auto-generated destructor stub
}

int GameServer::init(unsigned int _port) {
  coreServer->init(_port);
}

list<Group*> GameServer::findGroups(std::string _address) {
  return Server->findGroups(_address);
}

int GameServer::joinServerGroup(Group* _group) {
  serverGroup = _group;
  serverGroup->addGroup(this);
}

void GameServer::leaveServerGroup() {
  serverGroup->removeServer(this);
  serverGroup = NULL;
}

void GameServer::setObjectModel(GameObject* _objModel) {
  if (objectModel != NULL)
    delete objectModel;
  objectModel = _objModel;
}

void GameServer::setObjectModel(GameObject* _objModel) {
  objectModel = _objModel;
}
