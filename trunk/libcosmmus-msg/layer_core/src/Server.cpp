/*
 * Server.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Server.h"
#include "../include/OPMessage.h"

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

void Server::handlePeerMessage(Message* _msg) {

}

void Server::handleClientMessage(Message* _msg) {
  OPMessage* clientMessage = OPMessage::unpackFromNetwork(_msg);
  switch (clientMessage->getType()) {
    case OPMessageType::APP_MSG :
      callbackServer->handleClientMessage(clientMessage->getExtraPayload());
      break;
    case OPMessageType::CLIENT_CMD :
      Command* clientCommand = clientMessage->getCommandList().front();
      handleClientCommand(clientCommand);
      break;
    default:

      break;
  }
  delete clientMessage;
}

void Server::handleClientCommand(Command* _cmd) {
  if (_cmd->knowsTargets()) {

  }
  else if(_cmd->knowsGroups()) {

  }
  else return;
}

void Server::handleCommandInsideGroup(Command* _cmd) {

}

void Server::handleCommandToOtherGroups(Command* _cmd) {

}
