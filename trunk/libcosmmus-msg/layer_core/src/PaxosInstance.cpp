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
#include "../include/PaxosLearnerInterface.h"

using namespace optpaxos;
using namespace netwrapper;

UnreliablePeer* PaxosInstance::peerInterface = NULL;
PaxosLearnerInterface* PaxosInstance::callbackLearner = NULL;

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

void PaxosInstance::broadcast(OPMessage* _msg) {
  OPMessage* acceptMessage = new OPMessage();
  acceptMessage->setType(PAXOS_ACCEPT_MSG);
  acceptMessage->addMessage(PaxosInstance::packToNetwork(this));
  acceptMessage->addMessage(OPMessage::packToNetwork(_msg));
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

void PaxosInstance::handleAcceptMessage(OPMessage* _accMsg) {
  PaxosInstance* receivedInstance = PaxosInstance::unpackFromNetwork(_accMsg->getMessageList().front());
  OPMessage* broadcastValue = OPMessage::unpackFromNetwork(_accMsg->getMessageList().back());
  std::map<long, PaxosInstance*>::iterator it = instancesIndex.find(receivedInstance->getId());
  if (it != instancesIndex.end()) {
    PaxosInstance* instance = it->second;
    if ( !( broadcastValue->equals(instance->acceptedValue )) && instance->learnt) {
      instance->broadcastAcceptedValue();
    }
    delete receivedInstance;
    delete broadcastValue;
  }
  else {
    instancesIndex[receivedInstance->getId()] = receivedInstance;
    receivedInstance->acceptedValue = broadcastValue;
    receivedInstance->learnt = false;
    receivedInstance->acceptedMsgCounter = 0;
    receivedInstance->broadcastAcceptedValue();
  }
}

void PaxosInstance::broadcastAcceptedValue() {
  OPMessage* accedMsg = new OPMessage();
  accedMsg->setType(PAXOS_ACCEPTED_MSG);
  accedMsg->addMessage(PaxosInstance::packToNetwork(this));
  accedMsg->addMessage(OPMessage::packToNetwork(acceptedValue));
  Message* packedAccedMsg = OPMessage::packToNetwork(accedMsg);
  for (std::list<Group*>::iterator itgroup = learners.begin() ; itgroup != learners.end() ; itgroup++) {
    std::list<NodeInfo*> group = (*itgroup)->getServerList();
    for (std::list<NodeInfo*>::iterator itnode = group.begin() ; itnode != group.end() ; itnode++) {
      peerInterface->sendMessage(packedAccedMsg, (*itnode)->getAdress());
    }
  }
  delete packedAccedMsg;
  delete accedMsg;

}

void PaxosInstance::handleAcceptedMessage(OPMessage* _accedMsg) {
  PaxosInstance* receivedInstance = PaxosInstance::unpackFromNetwork(_accedMsg->getMessageList().front());
  OPMessage* broadcastValue = OPMessage::unpackFromNetwork(_accedMsg->getMessageList().back());
  std::map<long,PaxosInstance*>::iterator it = instancesIndex.find(receivedInstance->getId());
  if (it == instancesIndex.end()) {
    instancesIndex[receivedInstance->getId()] = receivedInstance;
    receivedInstance->acceptedValue = broadcastValue;
    receivedInstance->learnt = false;
    receivedInstance->acceptedMsgCounter = 1;
  }
  else {
    PaxosInstance* instance = it->second;

    // TODO: we are assuming, for now, that there is no byzantine behavior, that
    // is, if an ACC'ED msg is received, it's value is coherent with the others:

    instance->acceptedMsgCounter++;
    if (instance->acceptedMsgCounter > ((int) instance->acceptors.size()) / 2
        && instance->learnt == false) {
      instance->learnt = true;
      callbackLearner->handleLearntValue(instance->acceptedValue);
    }
    delete receivedInstance;
    delete broadcastValue;
  }
}

void PaxosInstance::setLearner(PaxosLearnerInterface* _learner) {
  callbackLearner = _learner;
}

void PaxosInstance::setPeerInterface(netwrapper::UnreliablePeer* _peer) {
  peerInterface = _peer;
}
