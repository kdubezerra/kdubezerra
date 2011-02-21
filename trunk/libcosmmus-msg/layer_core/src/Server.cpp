/*
 * Server.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Command.h"
#include "../include/Group.h"
#include "../include/NodeInfo.h"
#include "../include/Object.h"
#include "../include/ObjectInfo.h"
#include "../include/OPMessage.h"
#include "../include/PaxosInstance.h"
#include "../include/Server.h"
#include "../include/ServerInterface.h"

using namespace optpaxos;
using namespace netwrapper;

Server::Server() {
  netServer = new FIFOReliableServer();
  groupPeer = new UnreliablePeer();
  callbackServer = NULL;
  nodeInfo = NULL;
  lastPaxosInstance = 0;
  PaxosInstance::setLearner(this);
}

Server::~Server() {
  // TODO Auto-generated destructor stub
}

int Server::init(unsigned _reliablePort, unsigned _unreliablePort) {
  int returnValue;
  returnValue = netServer->init(_reliablePort);
  if (returnValue != 0) return returnValue;
  returnValue = groupPeer->init(_unreliablePort);
  if (returnValue != 0) return returnValue;
  PaxosInstance::setPeerInterface(groupPeer);
  PaxosInstance::setLearner(this);
  return 0;
}

int Server::joinGroup(Group *_group) {
  localGroup = _group;
  return 0;
  // TODO: create a decent membership manager
}

void Server::leaveGroup() {
  // TODO: create a decent membership manager
}

void Server::handleClientConnect(netwrapper::RemoteFRC* _newClient) {
  clientList.push_back(_newClient);
  // TODO: create a decent client manager
}

void Server::handleClientDisconnect(netwrapper::RemoteFRC* _client) {
  clientList.remove(_client);
  // TODO: create a decent membership manager (besides, the previous commline just doesn't work)
}

void Server::handleClientMessage(Message* _msg) {
  OPMessage* clientMessage = OPMessage::unpackFromNetwork(_msg);
  switch (clientMessage->getType()) {

    case APP_MSG :
      callbackServer->handleClientMessage(clientMessage);
      break;

    case CLIENT_CMD : {
      Command* clientCommand = clientMessage->getCommandList().front();
      if (clientCommand->knowsGroups() == false) clientCommand->findGroups();
      // TODO: fwdOptimisticallyToGroups(clientCommand);
      fwdCommandToCoordinator(clientCommand);
      break;
    }

    default:
      break;

  }
  delete clientMessage;
}

void Server::handlePeerMessage(Message* _msg) {
  OPMessage* peerMessage = OPMessage::unpackFromNetwork(_msg);
  switch (peerMessage->getType()) {

    case CMD_OPT :
      break;

    case CMD_TO_COORD : {
      OPMessage* cmdMessage = peerMessage;
      Command* cmd = cmdMessage->getCommandList().front();
      if (cmd->knowsGroups() == false) cmd->findGroups();
      if (cmd->getGroupList().size() == 1) {
        PaxosInstance* pxInstance = new PaxosInstance(++lastPaxosInstance * SRV_ID_LEN + (long) localGroup->getId());
        // TODO:
        cmd->calculateStamp();
        cmdMessage->setType(CMD_ONE_GROUP_CONSERVATIVE);
        pxInstance->addAcceptors(localGroup);
        pxInstance->addLearners(localGroup);
        pxInstance->broadcast(cmdMessage);
      }
      else {
        // TODO: find local targets for cmd
      }
      break;
    }

    case PAXOS_ACCEPT_MSG :
      PaxosInstance::handleAcceptMessage(peerMessage);
      break;

    case PAXOS_ACCEPTED_MSG :
      PaxosInstance::handleAcceptedMessage(peerMessage);
      break;

    default:
      break;

  }
  delete peerMessage;
}

void Server::handleLearntValue(OPMessage* _learntMsg) {
  switch (_learntMsg->getType()) {
    case CMD_ONE_GROUP_CONSERVATIVE : {
      Command* newCmd = _learntMsg->getCommandList().front();
      newCmd->setConservativelyDeliverable(true);
      sendCommandToClients(newCmd);
      std::list<ObjectInfo*> targetList = newCmd->getTargetList();
      for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++) {
        Object* obj = Object::getObjectById((*it)->getId());
        obj->enqueue(newCmd, CONSERVATIVE);
      }
      for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++) {
        Object* obj = Object::getObjectById((*it)->getId());
        obj->tryFlushingCmdQueue(CONSERVATIVE);
      }
      break;
    }
    default:
      break;
  }
  Command* cmd = _learntMsg->getCommandList().front();
  Object::handleCommand(cmd);
  sendCommandToClients(cmd);
}

void Server::sendCommandToClients(Command* _cmd) {
  // TODO : control subscribers of each command, so that some bandwidth can be saved
  OPMessage* cmdMsg = new OPMessage();
  cmdMsg->setType(CMD_ONE_GROUP_CONSERVATIVE);
  cmdMsg->addCommand(new Command(_cmd));
  Message* packedCmdMsg = OPMessage::packToNetwork(cmdMsg);
  for (std::list<netwrapper::RemoteFRC*>::iterator it = clientList.begin() ; it != clientList.end() ; it++) {
    netServer->send(packedCmdMsg, *it);
  }
}

void Server::fwdOptimisticallyToGroups(Command* _cmd) {
  // TODO: _cmd->setTimestamp(now + delay(this, coordinator))
  OPMessage* cmdOpMsg = new OPMessage();
  cmdOpMsg->setType(CMD_OPT);
  cmdOpMsg->addCommand(new Command(_cmd));
  Message* packedCmdOpMsg = OPMessage::packToNetwork(cmdOpMsg);
  std::list<Group*> groupList = _cmd->getGroupList();
  for (std::list<Group*>::iterator itgroup = groupList.begin() ; itgroup != groupList.end() ; itgroup++) {
    std::list<NodeInfo*> servers = (*itgroup)->getServerList();
    for (std::list<NodeInfo*>::iterator itserver = servers.begin() ; itserver != servers.end() ; itserver++) {
      groupPeer->sendMessage(packedCmdOpMsg, (*itserver)->getAdress());
    }
  }
  delete packedCmdOpMsg;
  delete cmdOpMsg;
}

void Server::fwdCommandToCoordinator(Command* _cmd) {
  OPMessage* cmdOpMsg = new OPMessage();
  cmdOpMsg->setType(CMD_TO_COORD);
  cmdOpMsg->addCommand(new Command(_cmd));
  Message* packedCmdOpMsg = OPMessage::packToNetwork(cmdOpMsg);
  groupPeer->sendMessage(packedCmdOpMsg, localGroup->getCoordinator()->getAdress());
  delete packedCmdOpMsg;
  delete cmdOpMsg;
}

void Server::handleCommandOneGroup(Command* _cmd) {
  /**
   send command to coordinator;
   if objects are not locked
   PaxosBCast(_cmd + _maxtsamongobjects, _group, _group) { // void PaxosBCast(what, acceptorsGroupsList, learnersGroupsList)
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
