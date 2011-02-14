/*
 * GameServer.cpp
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/GameServer.h"
#include "../include/GameObject.h"
#include "../include/Player.h"

#include "../layer_core/include/Server.h"

using namespace cosmmusmsg;
using namespace optpaxos;

GameServer::GameServer() {
  objectModel = NULL;
  serverGroup = NULL;
  coreServer = new Server();
  coreServer->setCallbackInterface(this);
}

GameServer::~GameServer() {
  // TODO Auto-generated destructor stub
}

int GameServer::init(unsigned int _port) {
  return coreServer->init(_port, _port);
}

std::list<Group*> GameServer::findGroups(std::string _address, int _port) {
  return Group::findGroups(_address, _port);
}

int GameServer::joinServerGroup(Group* _group) {
  serverGroup = _group;
  serverGroup->addServer(this->coreServer->getNodeInfo());
  return 0;
}

void GameServer::leaveServerGroup() {
  serverGroup->removeServer(this->coreServer->getNodeInfo());
  serverGroup = NULL;
}
