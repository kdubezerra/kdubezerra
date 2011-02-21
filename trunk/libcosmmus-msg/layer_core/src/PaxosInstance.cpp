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
std::map<long, PaxosInstance*> PaxosInstance::instancesIndex;


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

void PaxosInstance::flushToDisk() {

}

long PaxosInstance::getId() {
  return instanceSeq;
}

void PaxosInstance::setId(long _id) {
  instanceSeq = _id;
}

void PaxosInstance::addAcceptors(Group* _acceptors) {
  acceptorsGroupsList.push_back(_acceptors);
}

void PaxosInstance::setAcceptorsGroups(std::list<Group*> _accList) {
  acceptorsGroupsList = _accList;
}

std::list<Group*> PaxosInstance::getAcceptors() {
  return acceptorsGroupsList;
}

void PaxosInstance::addLearners(Group* _learners) {
  learnersGroupsList.push_back(_learners);
}

void PaxosInstance::setLearnersGroups(std::list<Group*> _lList) {
  learnersGroupsList = _lList;
}

std::list<Group*> PaxosInstance::getLearners() {
  return learnersGroupsList;
}

void PaxosInstance::broadcast(OPMessage* _msg) {
  OPMessage* acceptMessage = new OPMessage();
  acceptMessage->setType(PAXOS_ACCEPT_MSG);
  acceptMessage->addMessage(PaxosInstance::packToNetwork(this));
  acceptMessage->addMessage(OPMessage::packToNetwork(_msg));
  Message* packedAccMsg = OPMessage::packToNetwork(acceptMessage);
  for (std::list<Group*>::iterator itgroup = acceptorsGroupsList.begin() ; itgroup != acceptorsGroupsList.end() ; itgroup++) {
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
  for (std::list<Group*>::iterator itgroup = learnersGroupsList.begin() ; itgroup != learnersGroupsList.end() ; itgroup++) {
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
    if (instance->acceptedMsgCounter > ((int) instance->acceptorsGroupsList.size()) / 2
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

netwrapper::Message* PaxosInstance::packToNetwork(PaxosInstance* _instance) {
  Message* piMsg = new Message();
  piMsg->addInt(_instance->instanceSeq);
  piMsg->addMessage(Group::packListToNetwork(_instance->acceptorsGroupsList));
  piMsg->addMessage(Group::packListToNetwork(_instance->learnersGroupsList));
  return piMsg;
}

PaxosInstance* PaxosInstance::unpackFromNetwork(netwrapper::Message* _msg) {
  PaxosInstance* pi = new PaxosInstance();
  pi->setId(_msg->getInt(0));
  pi->setAcceptorsGroups(Group::unpackListFromNetwork(_msg->getMessage(0)));
  pi->setLearnersGroups(Group::unpackListFromNetwork(_msg->getMessage(1)));
  return pi;
}
