/*
 * Server.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Server.h"

using namespace optpaxos;
using namespace netwrapper;

Server::Server() {
  netServer = new FIFOReliableServer();
  groupPeer = new UnreliablePeer();
  objectFactory = NULL;
  callbackServer = NULL;
}

Server::~Server() {
  // TODO Auto-generated destructor stub
}

int Server::init(unsigned _reliablePort, unsigned _unreliablePort) {
  int returnValue;
  returnValue = netServer->init(_reliablePort);
  if (returnValue != NULL) return returnValue;
  returnValue = groupPeer->init(_unreliablePort);
  if (returnValue != NULL) return returnValue;
  return 0;
}

int Server::joinGroup(Group *_group) {

}

void Server::leaveGroup() {

}

void Server::setObjectFactory(ObjectFactory* _factory) {
  objectFactory = _factory;
}

void Server::handleClientMessage(netwrapper::Message* _msg) {
  /* TODO:
  if (command...)
  else callback...
  */
}

Message* Server::packToNetwork(Server* _obj) {
  Message* servInfo = new Message();

  return servInfo;
}

Message* Server::packServerListToNetwork(std::list<Server*> _objList) {
  Message* servListInfo = new Message();
  //TODO:
  return servListInfo;
}

SERVER_INFO Server::unpackFromNetwork(Message* _msg) {
  SERVER_INFO servInfo;
  //TODO:
  return servInfo;
}

std::list<SERVER_INFO> Server::unpackServerListFromNetwork(Message* _msg) {
  std::list<SERVER_INFO> serverInfoList;

  int count = _msg->getInt(0);
  for (int index = 0 ; index < count ; index++) {
    serverInfoList.push_back(Server::unpackFromNetwork(_msg->getMessage(index)));
  }

  return serverInfoList;
}

void Server::handleCommandInsideGroup(Command* _cmd) {

}

void Server::handleCommandToOtherGroups(Command* _cmd) {

}
