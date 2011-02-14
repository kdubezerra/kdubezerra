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
  callbackServer = NULL;
  nodeInfo = NULL;
  lastPaxosInstance = 0;
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
  PaxosInstance::setPeerInterface(groupPeer);
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
    case APP_MSG :
      callbackServer->handleClientMessage(clientMessage->getExtraPayload());
      break;
    case CLIENT_CMD :
      Command* clientCommand = clientMessage->getCommandList().front();
      if (clientCommand->knowsGroups() == false) clientCommand->findGroups();
      fwdOptimisticallyToGroups(clientCommand);
      fwdCommandToCoordinator(clientCommand);
      break;
    case CMD_OPT :

      break;
    case CMD_TO_COORD :
      Command* cmd = clientMessage->getCommandList().front();
      if (cmd->knowsGroups() == false) cmd->findGroups();
      if (cmd->getGroupList().size == 1) {
        PaxosInstance* pxInstance = new PaxosInstance(++lastPaxosInstance);
        //TODO:
        pxInstance->addAcceptors(localGroup);
        pxInstance->addLearners(localGroup);
        pxInstance->broadCast(cmd);
      }
      else {

      }
      break;
    case ACCEPT_MSG :
      PaxosInstance::handleAcceptMessage(_msg);
      break;
    case ACCEPTED_MSG :
      PaxosInstance::handleAcceptedMessage(_msg);
      break;
    default:

      break;
  }
  delete clientMessage;
}

void Server::fwdOptimisticallyToGroups(Command* _cmd) {
  Command* cmdCopy = new Command(_cmd);
  // TODO: cmdCopy->setTimestamp(now + delay(this, coordinator))
  OPMessage* cmdOpMsg = new OPMessage();
  cmdOpMsg->setType(CMD_OPT);
  cmdOpMsg->addCommand(cmdCopy);
  Message* packedCmdOpMsg = OPMessage::packToNetwork(cmdOpMsg);
  std::list<Group*> groupList = cmdCopy->getGroupList();
  for (std::list<Group*>::iterator itgroup = groupList.begin() ; itgroup != groupList.end() ; itgroup++) {
    std::list<NodeInfo> servers = (*itgroup)->getServerList();
    for (std::list<NodeInfo*>::iterator itserver = servers.begin() ; itserver != servers.end() ; itserver++) {
      groupPeer->sendMessage(packedCmdOpMsg, (*itserver)->getAdress());
    }
  }
  delete packedCmdOpMsg;
  delete cmdOpMsg;
}

void Server::fwdCommandToCoordinator(Command* _cmd) {
  NodeInfo* coordinator = localGroup->getCoordinator();
  Command* cmdCopy = new Command(_cmd);
  OPMessage* cmdOpMsg = new OPMessage();
  cmdOpMsg->setType(CMD_TO_COORD);
  cmdOpMsg->addCommand(cmdCopy);
  Message* packedCmdOpMsg = OPMessage::packToNetwork(cmdOpMsg);
  groupPeer->sendMessage(packedCmdOpMsg, coordinator->getAdress());
  delete packedCmdOpMsg;
  delete cmdOpMsg;
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
