/*
 * PaxosInstance.cpp
 *
 *  Created on: 10/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Command.h"
#include "../include/Group.h"
#include "../include/NodeInfo.h"
#include "../include/OPMessage.h"
#include "../include/PaxosInstance.h"

using namespace optpaxos;
using namespace netwrapper;

UnreliablePeer* peerInterface = NULL;

PaxosInstance::PaxosInstance() {
  init();
}

PaxosInstance::PaxosInstance(long _instanceId) {
  init();
  instanceSeq = _instanceId;
}

void PaxosInstance::init() {
  acceptedMsgCounter = 0;
  acceptedValue = NULL;
  learnt = false;
}

PaxosInstance::~PaxosInstance() {
  // TODO Auto-generated destructor stub
}

void PaxosInstance::broadcast(Command* _cmd) {
  OPMessage* acceptMessage = new OPMessage();
  acceptMessage->setType(ACCEPT_MSG);
  acceptMessage->addCommand(new Command(_cmd));
  acceptMessage->setExtraPayload(PaxosInstance::packToNetwork(this));
  Message* packedAccMsg = OPMessage::packToNetwork(acceptMessage);
  for (std::list<Group*>::iterator itgroup = acceptors.begin() ; itgroup != acceptors.end() ; itgroup++) {
    std::list<NodeInfo*> group = (*itgroup)->getServerList();
    for (std::list<NodeInfo*>::iterator itnode = group.begin() ; itnode != group.end() ; itnode++) {
      peerInterface->sendMessage(packedAccMsg, (*itnode)->getAdress());
    }
  }
  delete packedAccMsg;
  delete acceptMessage;
}

void PaxosInstance::broadcastAcceptedValue() {
  OPMessage* accedMsg = acceptedValue;
  accedMsg->setType(ACCEPTED_MSG);
  Message* packedAccedMsg = OPMessage::packToNetwork(accedMsg);
  for (std::list<Group*>::iterator itgroup = acceptors.begin() ; itgroup != acceptors.end() ; itgroup++) {
    std::list<NodeInfo*> group = (*itgroup)->getServerList();
    for (std::list<NodeInfo*>::iterator itnode = group.begin() ; itnode != group.end() ; itnode++) {
      peerInterface->sendMessage(packedAccedMsg, (*itnode)->getAdress());
    }
  }
  delete packedAccedMsg;
}

void PaxosInstance::broadcastToLearners() {
  OPMessage* learnMsg = acceptedValue;
  learnMsg->setType(LEARN_MSG);
  Message* packedLearnMsg = OPMessage::packToNetwork(learnMsg);
  for (std::list<Group*>::iterator itgroup = learners.begin() ; itgroup != learners.end() ; itgroup++) {
    std::list<NodeInfo*> group = (*itgroup)->getServerList();
    for (std::list<NodeInfo*>::iterator itnode = group.begin() ; itnode != group.end() ; itnode++) {
      peerInterface->sendMessage(packedLearnMsg, (*itnode)->getAdress());
    }
  }
  delete packedLearnMsg;
}

void PaxosInstance::setPeerInterface(netwrapper::UnreliablePeer* _peer) {
  peerInterface = _peer;
}

void PaxosInstance::handleAcceptMessage(OPMessage* _accMsg) {

  PaxosInstance* receivedInstance = PaxosInstance::unpackFromNetwork(_accMsg->getExtraPayload());
  std::map<long, PaxosInstance*>::iterator it = instancesIndex.find(receivedInstance->getId());
  if (it != instancesIndex.end()) {
    if ( !( _accMsg->getCommandList().front()->equals(it->second->acceptedValue->getCommandList().front()) )
        && it->second->learnt) {
      it->second->broadcastAcceptedValue();
    }
    delete receivedInstance;
  }
  else {
    instancesIndex[receivedInstance->getId()] = receivedInstance;
    receivedInstance->acceptedValue = new OPMessage();
    receivedInstance->acceptedValue->addCommand(new Command(_accMsg->getCommandList().front()));
    receivedInstance->learnt = false;
    receivedInstance->broadcastAcceptedValue();
  }

}

void PaxosInstance::handleAcceptedMessage(OPMessage* _accedMsg) {
  PaxosInstance* receivedInstance = PaxosInstance::unpackFromNetwork(_accedMsg->getExtraPayload());
  std::map<long, PaxosInstance*>::iterator it = instancesIndex.find(receivedInstance->getId());
  if (it != instancesIndex.end()) {
    PaxosInstance* instance = it->second;
    instance->acceptedMsgCounter++;
    if (instance->acceptedMsgCounter > ((int) instance->acceptors.size()) / 2
        && !instance->learnt) {
      instance->learnt = true;
      instance->broadcastToLearners();
    }
  }
}
