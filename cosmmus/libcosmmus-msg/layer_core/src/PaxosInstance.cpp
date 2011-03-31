/*
 * PaxosInstance.cpp
 *
 *  Created on: 10/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>
#include "../include/Command.h"
#include "../include/Group.h"
#include "../include/NodeInfo.h"
#include "../include/OPMessage.h"
#include "../include/PaxosInstance.h"
#include "../include/PaxosLearnerInterface.h"


using namespace std;
using namespace optpaxos;
using namespace netwrapper;

UnreliablePeer* PaxosInstance::peerInterface = NULL;
PaxosLearnerInterface* PaxosInstance::callbackLearner = NULL;
std::map<long, PaxosInstance*> PaxosInstance::instancesIndex;
std::list<PaxosInstance*> PaxosInstance::flushableList;


PaxosInstance::PaxosInstance() {
  init();
  instanceSeq = -1;
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
  if (acceptedValue != NULL)
    delete acceptedValue;
  for (std::list<Group*>::iterator it = acceptorsGroupsList.begin() ; it != acceptorsGroupsList.end() ; it++) {
    delete *it;
  }
  for (std::list<Group*>::iterator it = learnersGroupsList.begin() ; it != learnersGroupsList.end() ; it++) {
    delete *it;
  }
}

void PaxosInstance::flushToDisk(PaxosInstance* _pi) {
  // TODO: actually should SAVE to disk!
  //cout << "PaxosInstance::flushToDisk: flushing instance " << _pi->instanceSeq << endl;
  instancesIndex.erase(_pi->getId());
  flushableList.remove(_pi);
  delete _pi;
}

void PaxosInstance::flushAll() {
  std::list<PaxosInstance*> flushable = flushableList;
  for (std::list<PaxosInstance*>::iterator it = flushable.begin() ; it != flushable.end() ; it++) {
    flushToDisk(*it);
  }
  //cout << "PaxosInstance::flushToDisk: remaining instances: " << flushableList.size() << endl;
}

PaxosInstance* PaxosInstance::findInstance(long _seq) {
  // TODO: if the instance is not in memory, search in the disk, as it must have been flushed to it
  std::map<long, PaxosInstance*>::iterator finder = instancesIndex.find(_seq);
  if (finder == instancesIndex.end())
    return NULL;
  else
    return finder->second;
}

long PaxosInstance::getId() {
  return instanceSeq;
}

void PaxosInstance::setId(long _id) {
  instanceSeq = _id;
}

void PaxosInstance::addAcceptors(Group* _acceptors) {
  acceptorsGroupsList.push_back(new Group(_acceptors));
}

void PaxosInstance::setAcceptorsGroups(std::list<Group*> _accList) {
  for (std::list<Group*>::iterator it = _accList.begin() ; it != _accList.end() ; it++)
    addAcceptors(*it);
}

std::list<Group*> PaxosInstance::getAcceptors() {
  return acceptorsGroupsList;
}

void PaxosInstance::addLearners(Group* _learners) {
  learnersGroupsList.push_back(new Group(_learners));
}

void PaxosInstance::setLearnersGroups(std::list<Group*> _lList) {
  for (std::list<Group*>::iterator it = _lList.begin() ; it != _lList.end() ; it++)
    addLearners(*it);
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
  PaxosInstance* rcvdInstance = PaxosInstance::unpackFromNetwork(_accMsg->getMessageList().front());
  OPMessage* rcvdValue = OPMessage::unpackFromNetwork(_accMsg->getMessageList().back());
  PaxosInstance* localInstance = PaxosInstance::findInstance(rcvdInstance->getId());
  if (localInstance != NULL) {
    //cout << "PaxosInstance::handleAcceptMessage: beginning with already existing instance" << endl;
    if ( !rcvdValue->equals(localInstance->acceptedValue) ) {
      localInstance->broadcastAcceptedValue();
    }
    delete rcvdInstance;
    delete rcvdValue;
  }
  else {
    //cout << "PaxosInstance::handleAcceptMessage: beginning with new instance" << endl;
    instancesIndex[rcvdInstance->getId()] = rcvdInstance;
    rcvdInstance->acceptedValue = rcvdValue;
    rcvdInstance->learnt = false;
    rcvdInstance->acceptedMsgCounter = 0;
    rcvdInstance->broadcastAcceptedValue();
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
  PaxosInstance* rcvdInstance = PaxosInstance::unpackFromNetwork(_accedMsg->getMessageList().front());
  OPMessage* rcvdValue = OPMessage::unpackFromNetwork(_accedMsg->getMessageList().back());
  PaxosInstance* localInstance = PaxosInstance::findInstance(rcvdInstance->getId());
  if (localInstance == NULL) {
    instancesIndex[rcvdInstance->getId()] = rcvdInstance;
    rcvdInstance->acceptedValue = rcvdValue;
    rcvdInstance->learnt = false;
    rcvdInstance->acceptedMsgCounter = 1;
  }
  else {

    // TODO: we are assuming, for now, that there is no byzantine behavior, that
    // is, if an ACC'ED msg is received, it's value is coherent with the others:

    localInstance->acceptedMsgCounter++;
    int numAcceptors = 0;
    for (std::list<Group*>::iterator it = localInstance->acceptorsGroupsList.begin() ; it != localInstance->acceptorsGroupsList.end() ; it++)
      numAcceptors += (int) (*it)->getServerList().size();
    int mostAcceptors = (numAcceptors + 1) % 2 ? (numAcceptors + 2) / 2 : (numAcceptors + 1) / 2;
    //cout << "PaxosInstance::handleAcceptedMessage: numAcceptors = " << numAcceptors << ", acceptedMsgCounter = " << localInstance->acceptedMsgCounter << endl;
    if (localInstance->learnt == false && localInstance->acceptedMsgCounter >= mostAcceptors) { // TODO: fix this to deliver only once
      localInstance->learnt = true;
      callbackLearner->handleLearntValue(localInstance->acceptedValue);
    }
    if (localInstance->acceptedMsgCounter == numAcceptors) // TODO: create a better criterion to mark an instance as flushable
      flushableList.push_back(localInstance);
    delete rcvdInstance;
    delete rcvdValue;
  }
}

void PaxosInstance::setLearner(PaxosLearnerInterface* _learner) {
  callbackLearner = _learner;
}

void PaxosInstance::setPeerInterface(netwrapper::UnreliablePeer* _peer) {
  peerInterface = _peer;
}

Message* PaxosInstance::packToNetwork(PaxosInstance* _instance) {
  Message* piMsg = new Message();
  piMsg->addInt(_instance->instanceSeq);
  piMsg->addMessage(Group::packListToNetwork(_instance->acceptorsGroupsList));
  piMsg->addMessage(Group::packListToNetwork(_instance->learnersGroupsList));
  return piMsg;
}

PaxosInstance* PaxosInstance::unpackFromNetwork(netwrapper::Message* _msg) {
  PaxosInstance* pi = new PaxosInstance();
  pi->instanceSeq = _msg->getInt(0);
  pi->acceptorsGroupsList = Group::unpackListFromNetwork(_msg->getMessage(0));
  pi->learnersGroupsList = Group::unpackListFromNetwork(_msg->getMessage(1));
  return pi;
}
