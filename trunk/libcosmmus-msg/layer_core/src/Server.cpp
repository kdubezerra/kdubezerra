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
  addTimeStamp(_cmd);

  if (_cmd->knowsTargets()) {
    int groupCount = (int) (_cmd->findGroups().size());
    if (groupCount == 1)
      handleCommandOneGroup(_cmd);
    if (groupCount > 1)
      handleCommandMultipleGroups(_cmd);
  }
  else if(_cmd->knowsGroups()) {

  }
  else return;
}

void Server::handleCommandOneGroup(Command* _cmd) {
  /**
   send command to coordinator;
   if objects are not locked
   PaxosBCast(_cmd + _maxtsamongobjects, _group, _group) { // void PaxosBCast(what, acceptors, learners)
     if (this is coordinator) {
       send command to all members of the group;
     }
     ...
     after receiving n/2 + 1 replies, deliver it.
   }
   */

}

void Server::handleCommandMultipleGroups(Command* _cmd) {
  /**
   1) send command to coordinator;
   2) coordinator fwd's the command to each coordinator of the groups
   3...) each coordinator [finds the targets of the command and], based on the command's targets defines a stamp proposal for the command and ...
   PaxosBCast(command + proposal, _group, _group1 + _group2 + ... + _groupN) {
     ...3) ...sends to its local group's members
        4) when each member of the group receives the proposal, it sends an accept to ALL servers involved (s : s /in g1, g2, ..., gn)
        5...) when a server has received an "ACCEPTED." message from the majority of members
   }
...5) of every group for each message, then it calculates the max_proposal() [and assemble the target list from all servers]
   6) if the max_proposal() is the one proposed by its group
     6.1) the server immediately delivers the command and increase the logical clock by 1
     6.2...) else, first it
     PaxosBCast(newclock_value, _group, _group) {
        ...6.2...) proposes the new clock value to the group...
     }
  ...6.2) and, then, delivers the command.
  **/
}
