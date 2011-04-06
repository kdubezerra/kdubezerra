/*
 * Object.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>
#include "../include/Command.h"
#include "../include/Object.h"
#include "../include/ObjectFactory.h"
#include "../include/ObjectInfo.h"
#include "../include/Server.h"

using namespace std;
using namespace optpaxos;
using namespace netwrapper;

std::map<int, Object*> Object::objectIndex;
ObjectFactory* Object::objectFactory = NULL;
Server* Object::callbackServer = NULL;


Object::Object() {
  objectInfo = NULL;
  lockedForProposals = false;
}


Object::Object(Object* _other) {
  objectInfo = new ObjectInfo(_other->objectInfo);
  lockedForProposals = false;
}


Object::Object(int _id) {
  objectInfo = new ObjectInfo();
  objectInfo->setId(_id);
  objectInfo->setLastStamp(-1);
  lockedForProposals = false;
}


Object::~Object() {
  if (objectInfo != NULL)
    delete objectInfo;

  for (std::list<Command*>::iterator it = consCmdQueue.begin() ; it != consCmdQueue.end() ; it++)
    delete *it;
}


bool Object::equals(Object* _other) {
  if (this->objectInfo->getId() != _other->objectInfo->getId())
    return false;
  // TODO call the subclass' equals method
  return true;
}


ObjectInfo* Object::getInfo() {
  return objectInfo;
}


void Object::setInfo(ObjectInfo* _objInfo) {
  if (objectInfo != NULL)
    delete objectInfo;
  objectInfo = new ObjectInfo(_objInfo);
}


Object* Object::getObjectById(int _id) {
  std::map<int, Object*>::iterator finder = objectIndex.find(_id);
  if (finder != objectIndex.end())
    return finder->second;
  else
    return NULL;
}


void Object::indexObject(Object* _obj) {
  objectIndex[_obj->getInfo()->getId()] = _obj;
}


void Object::setObjectFactory(ObjectFactory* _factory) {
  objectFactory = _factory;
}


ObjectFactory* Object::getObjectFactory() {
  return objectFactory;
}


void Object::lock() {
  lockedForProposals = true;
}


void Object::unlock() {
  lockedForProposals = false;
}


bool Object::isLocked() {
  return lockedForProposals;
}


std::list<Command*> Object::getOptCmdQueue() {
  return optCmdQueue;
}


void Object::enqueueOrUpdateOptQueue(Command* _cmd) {
  for (std::list<Command*>::iterator it = optCmdQueue.begin() ; it != optCmdQueue.end() ; it++)
    if ((*it)->getId() == _cmd->getId()) {
      delete *it;
      optCmdQueue.erase(it);
      break;
    }

  optCmdQueue.push_back(new Command(_cmd));
  optCmdQueue.sort(Command::compareTimeStampThenId);
}


void Object::tryFlushingOptQueue() {
  if (optCmdQueue.empty()) {
    //cout << "Object::tryFlushingOptQueue: opt command queue of object " << this->getInfo()->getId() << " is CLEAR." << endl;
    return;
  }
  else {
    //cout << "Object::tryFlushingOptQueue: opt command queue of object " << this->getInfo()->getId() << " has " << (int) optCmdQueue.size() << " elements." << endl;
  }
  Command* nextCmd = new Command(optCmdQueue.front());

  std::list<ObjectInfo*> targetList = nextCmd->getTargetList();

  bool allObjsReady = true;

  for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
    Object* obj = Object::getObjectById((*ittarget)->getId());
    if (obj->optCmdQueue.front()->getId() != nextCmd->getId()) {
      allObjsReady = false;
      break;
    }
  }

  if (allObjsReady) {
    //cout << "Object::tryFlushingOptQueue: command " << nextCmd->getId() << " delivered (OPT) to object " << this->getInfo()->getId() << endl;
    for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
      Object* obj = Object::getObjectById((*ittarget)->getId());
      obj->handleOptimisticDelivery(nextCmd);
      delete obj->optCmdQueue.front();
      obj->optCmdQueue.pop_front();
    }

    for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
      Object* obj = Object::getObjectById((*ittarget)->getId());
      obj->tryFlushingOptQueue();
    }
  }

  delete nextCmd;
}


std::list<Command*> Object::getConsCmdQueue() {
  return consCmdQueue;
}


void Object::enqueueOrUpdateConsQueue(Command* _cmd) {
  for (std::list<Command*>::iterator it = consCmdQueue.begin() ; it != consCmdQueue.end() ; it++)
    if ((*it)->getId() == _cmd->getId()) {
      delete *it;
      consCmdQueue.erase(it);
      break;
    }

  consCmdQueue.push_back(new Command(_cmd));
  consCmdQueue.sort(Command::compareLogicalStampThenId);
}


void Object::tryFlushingConsQueue() { // TODO: seria melhor checar se o comando na frente da fila é o mesmo (e nao se o laststamp é o mesmo)
  consCmdQueue.sort(Command::compareLogicalStampThenId);
  if (consCmdQueue.empty()) {
    //cout << "Object::tryFlushingConsQueue: command queue of object " << this->getInfo()->getId() << " is CLEAR." << endl;
    return;
  }
  else {
    //cout << "Object::tryFlushingConsQueue: command queue of object " << this->getInfo()->getId() << " has " << (int) consCmdQueue.size() << " elements." << endl;
  }
  Command* nextCmd = new Command(consCmdQueue.front());

  if (nextCmd->getStage() != DELIVERABLE) {
    delete nextCmd;
    return;
  }
  std::list<ObjectInfo*> targetList = nextCmd->getTargetList();

  bool allObjsReady = true;

  for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
    Object* obj = Object::getObjectById((*ittarget)->getId());
    if (obj->getConsCmdQueue().front()->getId() != nextCmd->getId()) {
      allObjsReady = false;
      break;
    }
  }

  if (allObjsReady) {
    callbackServer->handleCommandReady(nextCmd);
    //cout << "Object::tryFlushingConsQueue: command " << nextCmd->getId() << " delivered to object " << this->getInfo()->getId() << endl;
    for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
      Object* obj = Object::getObjectById((*ittarget)->getId());
      obj->unlock();
      obj->handleConservativeDelivery(nextCmd);
      obj->objectInfo->setLastStamp(nextCmd->getLogicalStamp());
      delete obj->consCmdQueue.front();
      obj->consCmdQueue.pop_front();
      //cout << "Object::tryFlushingConsQueue: deleted command and removed from the queue of object " << obj->getInfo()->getId() << endl;
    }

    for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
      Object* obj = Object::getObjectById((*ittarget)->getId());
      obj->tryFlushingConsQueue();
    }
  }

  delete nextCmd;
}


void Object::setCallbackServer(Server* _server) {
  callbackServer = _server;
}


/*!
 * \brief This method is responsible for listing the objects affected by the command. Also,
 * it checks whether the command's delivery is either optimistic or conservative, and delivers
 * to the correspondent application level queue of each object. If the client doesn't have an
 * up-to-date state of all objects affected by the command, this method also forwards the
 * newest state (prior to the execution of the command) to each object needing update.
 * \param _cmd The core layer level command to be parsed.
 */
void Object::handleCommand(Command* _cmd) {
  std::list<Object*> priorStates = _cmd->getPriorStateList();
  std::list<ObjectInfo*> targets = _cmd->getTargetList();

  for (std::list<Object*>::iterator it = priorStates.begin() ; it != priorStates.end() ; it++) {
    Object* obj = Object::getObjectById((*it)->getInfo()->getId());
    if (_cmd->isOptimisticallyDeliverable())
      obj->handleNewOptimisticState(*it);
    if (_cmd->isConservativelyDeliverable())
      obj->handleNewConservativeState(*it);
  }

  for (std::list<ObjectInfo*>::iterator it = targets.begin() ; it != targets.end() ; it++) {
    Object* obj = Object::getObjectById((*it)->getId());
    if (_cmd->isOptimisticallyDeliverable())
      obj->handleOptimisticDelivery(_cmd);
    if (_cmd->isConservativelyDeliverable())
      obj->handleConservativeDelivery(_cmd);
      // TODO: at this point, the delivered command must be checked against the first one in the optimistic queue.
      //       Should be found a difference, action must be taken to correct this discrepancy.
      // default: application handles it.
  }
}


Object* Object::createObject() {
    return objectFactory->createObject();
}


Object* Object::copyObject(Object* _other) {
    return objectFactory->copyObject(_other);
}


Message* Object::packToNetwork(Object* _obj) {
  Message* msg = new Message();
  msg->addMessage(objectFactory->packToNetwork(_obj));
  msg->addMessage(ObjectInfo::packToNetwork(_obj->objectInfo));
  return msg;
}


Message* Object::packListToNetwork(std::list<Object*> _objList) {
  Message* objListMsg = new Message();

  objListMsg->addInt((int) _objList.size());
  for (std::list<Object*>::iterator it = _objList.begin() ; it != _objList.end() ; it++)
    objListMsg->addMessage(Object::packToNetwork(*it));

  return objListMsg;
}


Object* Object::unpackFromNetwork(Message* _msg) {
  Object* obj;
  obj = objectFactory->unpackFromNetwork(_msg->getMessage(0));
  obj->objectInfo = ObjectInfo::unpackFromNetwork(_msg->getMessage(1));
  return obj;
}


std::list<Object*> Object::unpackListFromNetwork(Message* _msg) {
  std::list<Object*> objList;

  int objectCount = _msg->getInt(0);
  for (int index = 0 ; index < objectCount ; index++)
    objList.push_back(Object::unpackFromNetwork(_msg->getMessage(index)));

  return objList;
}
