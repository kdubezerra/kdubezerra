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
  // TODO Auto-generated constructor stub

}

GameServer::~GameServer() {
  // TODO Auto-generated destructor stub
}

void GameServer::setObjectModel(Object* _objModel) {
  objectModel = _objModel;
}
