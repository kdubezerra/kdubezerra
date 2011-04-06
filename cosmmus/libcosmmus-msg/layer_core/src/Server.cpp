/*
 * Server.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>
#include <sys/time.h>
#include <time.h>
#include "../include/Command.h"
#include "../include/Group.h"
#include "../include/NodeInfo.h"
#include "../include/Object.h"
#include "../include/ObjectInfo.h"
#include "../include/OPMessage.h"
#include "../include/PaxosInstance.h"
#include "../include/Server.h"
#include "../include/ServerInterface.h"

using namespace std;
using namespace optpaxos;
using namespace netwrapper;


Server::Server() {
  netServer = new FIFOReliableServer();
  netServer->setCallbackInterface(this);
  groupPeer = new UnreliablePeer();
  groupPeer->setCallbackInterface(this);
  callbackGameServer = NULL;
  //nodeInfo = NULL;
  lastCommandId = lastPaxosInstance = 0;
  waitWindow = 0;
  PaxosInstance::setLearner(this);
  Object::setCallbackServer(this);
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
  localGroup = NULL;
  // TODO: create a decent membership manager
}


/*NodeInfo* Server::getNodeInfo() {
  return nodeInfo;
}


void Server::setNodeInfo(NodeInfo* _info) {
  nodeInfo = _info;
}*/


void Server::checkAll() {
  checkConnections();
  checkNewMessages();
  flushOptCmdQueue();
  tryProposingPendingCommands();
}


int Server::checkNewMessages() {
  return groupPeer->checkNewMessages() + netServer->checkNewMessages();
}


int Server::checkConnections() {
  // TODO: handle membership, group changes, disconnected servers etc.
  return netServer->checkConnections();
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
      callbackGameServer->handleClientApplicationMessage(clientMessage->getMessageList().front());
      break;

    case CLIENT_CMD : {
      Command* clientCommand = clientMessage->getCommandList().front();
      if (clientCommand->knowsGroups() == false) clientCommand->findGroups();
      fwdCommandOptimistically(clientCommand);
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

    case CMD_OPT : {
      Command* cmd = peerMessage->getCommandList().front();
      if (cmd->knowsGroups() == false) cmd->findGroups();
      if (cmd->getGroupList().size() == 1) {
        cmd->setOptimisticallyDeliverable(false);
        if (waitWindow < getTime() - cmd->getTimeStamp()) {
          //waitWindow = getTime() - cmd->getTimeStamp();
          //cout << "Server::handlePeerMessage: waitWindow increased to " << waitWindow << endl;
        }
        enqueueOptCmd(cmd);
      }
      break;
    }

    case CMD_TO_COORD : {
      OPMessage* cmdMessage = peerMessage;
      Command* cmd = cmdMessage->getCommandList().front();
      //cmd->setId(++lastCommandId * GRP_ID_LEN + (long) localGroup->getId()); // TODO: maybe this is not the best place to set an id for the command
      if (cmd->knowsGroups() == false) cmd->findGroups();
      //TODO: if(cmd->knowsTargets() == false) do something about it.
      if (cmd->getGroupList().size() == 1) {
        cmd->setConservativelyDeliverable(false);
        cmd->setStage(PROPOSING_LOCAL);
        cmd->calcLogicalStamp();
        std::list<ObjectInfo*> targetList = cmd->getTargetList();
        for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++) {
          Object* obj = Object::getObjectById((*it)->getId());
          obj->enqueueOrUpdateConsQueue(cmd);
        }
        while(tryProposingPendingCommands() > 0);
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
      PaxosInstance::flushAll();
      break;

    default:
      break;

  }

  delete peerMessage;

}


void Server::handleCommandReady(Command* _cmd) {
  sendCommandToClients(_cmd, CONSERVATIVE);
}


void Server::sendCommand(Command* cmd, long _clSeq, int _clId) {
  if (cmd->knowsGroups() == false) cmd->findGroups();
  // TODO: the client is supposed to assign a sequence # to its message... the server just makes it unique appending the client's id
  cmd->setId(_clSeq * 10 + _clId);
  fwdCommandOptimistically(cmd);
  fwdCommandToCoordinator(cmd);
}


//When Exists m in PENDING : m.stage = s0 || m.stage = s2) && propK < or = K
//
int Server::tryProposingPendingCommands() {
  int proposalsMade = 0;
  static int createdPxInstances = 0;

  std::list<ObjectInfo*> localObjects = localGroup->getObjectsList();
  for (std::list<ObjectInfo*>::iterator ito = localObjects.begin() ; ito != localObjects.end() ; ito++) {
    Object* obj = Object::getObjectById((*ito)->getId());
    std::list<Command*> objCmds = obj->getConsCmdQueue();
    for (std::list<Command*>::iterator itc = objCmds.begin() ; itc != objCmds.end() ; itc++) {
      if ((*itc)->getStage() != PROPOSING_LOCAL && (*itc)->getStage() != UPDATING_CLOCK)
        continue;
      std::list<ObjectInfo*> cmdTargets = (*itc)->getTargetList();
      bool cmdIsProposable = true;
      for (std::list<ObjectInfo*>::iterator itt = cmdTargets.begin() ; itt != cmdTargets.end() ; itt++) {
        Object* obj = Object::getObjectById((*itt)->getId());

        if (obj->getInfo()->getNextStamp() > obj->getInfo()->getClock()) //propk must be < or = K
          cmdIsProposable = false;

        if (obj->isLocked())
          cmdIsProposable = false;
      }
      if (!cmdIsProposable)
        continue;
      //cout << "Server::tryProposingPendingCommands: command " << (*itc)->getId() << " is proposable" << endl;
      (*itc)->calcLogicalStamp();
      for (std::list<ObjectInfo*>::iterator itt = cmdTargets.begin() ; itt != cmdTargets.end() ; itt++) {
        Object* obj = Object::getObjectById((*itt)->getId());
        obj->lock();
        obj->getInfo()->setNextStamp((*itc)->getLogicalStamp() + 1);
      }
      if ((*itc)->getGroupList().size() == 1) {
        cout << "createdPxInstances = " << ++createdPxInstances << endl;
        PaxosInstance* pxInstance = new PaxosInstance(++lastPaxosInstance * GRP_ID_LEN + (long) localGroup->getId());
        OPMessage* cmdMessage = new OPMessage();
        cmdMessage->setType(CMD_ONE_GROUP_CONSERVATIVE);
        cmdMessage->addCommand(new Command(*itc));
        pxInstance->addAcceptors(localGroup);
        pxInstance->addLearners(localGroup);
        pxInstance->broadcast(cmdMessage);
        proposalsMade++;
        delete cmdMessage;
        delete pxInstance;
      }
      //else if (cmd->knowsTargets())...;
    }
  }

  return proposalsMade;
}


void Server::handleLearntValue(OPMessage* _learntMsg) {
  switch (_learntMsg->getType()) {
    case CMD_ONE_GROUP_CONSERVATIVE : {
      Command* newCmd = _learntMsg->getCommandList().front();
      newCmd->setConservativelyDeliverable(true);
      newCmd->calcLogicalStamp(); //m.ts <- max(k), Oi in m's targets
      newCmd->setStage(DELIVERABLE); //m.stage = s3
      std::list<ObjectInfo*> targetList = newCmd->getTargetList();
      for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++) {
        Object* obj = Object::getObjectById((*it)->getId());
        if (obj == NULL); //cout << "Server::handleLearntValue: obj = NULL" << endl;
        else {
          obj->enqueueOrUpdateConsQueue(newCmd);
          obj->getInfo()->setClock(newCmd->getLogicalStamp() + 1);
          //obj->unlock();
        }
      }
      for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++) {
        Object* obj = Object::getObjectById((*it)->getId());
        obj->tryFlushingConsQueue();
      }
      break;
    }
    default:
      break;
  }
}


void Server::sendCommandToClients(Command* _cmd, CommandType _cmdType) {
  static int sentmsgs = 0;
  // TODO : control subscribers of each command, so that some bandwidth can be saved
  OPMessage* cmdMsg = new OPMessage();
  if (_cmdType == OPTIMISTIC) {
    if (_cmd->getGroupList().size() == 1)
      cmdMsg->setType(CMD_ONE_GROUP_OPTIMISTIC);
    else
      cmdMsg->setType(CMD_MULTI_GROUP_OPTIMISTIC);
  }
  else if (_cmdType == CONSERVATIVE) {
    if (_cmd->getGroupList().size() == 1)
      cmdMsg->setType(CMD_ONE_GROUP_CONSERVATIVE);
    else
      cmdMsg->setType(CMD_MULTI_GROUP_CONSERVATIVE);
  }
  cmdMsg->addCommand(new Command(_cmd));
  Message* packedCmdMsg = OPMessage::packToNetwork(cmdMsg);
  cout << "Server::sendCommandToClients: should be sending command to all of its clients" << endl;
  int i = 0;
  for (std::list<netwrapper::RemoteFRC*>::iterator it = clientList.begin() ; it != clientList.end() ; it++) {
    sentmsgs++;
    cout << "Server::sendCommandToClients: sending command to client " << ++i << " of " << clientList.size() << endl;
    cout << (_cmd->isConservativelyDeliverable() ? "CONSERVATIVE" : "OPTIMISTIC") << endl;
    netServer->send(packedCmdMsg, *it);
  }
  cout << "Server::sendCommandToClients: sentmsgs = " << sentmsgs << endl;
  delete packedCmdMsg;
  delete cmdMsg;
}


void Server::fwdCommandOptimistically(Command* _cmd) {
  _cmd->setTimeStamp(getTime()); // should be more like _cmd->setTimestamp(now + delay(this, coordinator))
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
   3...) each coordinator [finds the targets of the command and], based on the command's targets defines a logicalStamp proposal for the command and ...
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


long Server::getTime() {
  struct timeval tv;
  long timems;
  gettimeofday(&tv, NULL);
  timems = (tv.tv_sec - 1300196465)*1000 + (tv.tv_usec / 1000);
  return timems;
}


void Server::enqueueOptCmd(Command* _cmd) {
  for (std::list<Command*>::iterator it = optDeliveryQueue.begin() ; it != optDeliveryQueue.end() ; it++)
    if (_cmd->getId() == (*it)->getId()) {
      delete *it;
      optDeliveryQueue.erase(it);
      break;
    }

  optDeliveryQueue.push_back(new Command(_cmd));
  optDeliveryQueue.sort(Command::compareTimeStampThenId);
}


void Server::flushOptCmdQueue() {
  std::list<Command*> optQCopy = optDeliveryQueue;
  for (std::list<Command*>::iterator itc = optQCopy.begin() ; itc != optQCopy.end() ; itc++) {
    if (getTime() - (*itc)->getTimeStamp() > waitWindow) {
      std::list<ObjectInfo*> targetList = (*itc)->getTargetList();
      sendCommandToClients(*itc, OPTIMISTIC);
      for (std::list<ObjectInfo*>::iterator itt = targetList.begin() ; itt != targetList.end() ; itt++) {
        Object* obj = Object::getObjectById((*itt)->getId());
        obj->enqueueOrUpdateOptQueue(*itc);
      }
      for (std::list<ObjectInfo*>::iterator itt = targetList.begin() ; itt != targetList.end() ; itt++) {
        Object* obj = Object::getObjectById((*itt)->getId());
        obj->tryFlushingOptQueue();
      }
      delete optDeliveryQueue.front();
      optDeliveryQueue.pop_front();
    }
    else break;
  }
}
