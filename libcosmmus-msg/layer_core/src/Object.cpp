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

using namespace std;
using namespace optpaxos;
using namespace netwrapper;

std::map<int, Object*> Object::objectIndex;
ObjectFactory* Object::objectFactory = NULL;

Object::Object() {
  objectInfo = NULL;
  waitingForDecision = false;
}

Object::Object(Object* _other) {
  objectInfo = new ObjectInfo(_other->objectInfo);
  waitingForDecision = false;
}

Object::Object(int _id) {
  objectInfo = new ObjectInfo();
  objectInfo->setId(_id);
  objectInfo->setLastStamp(-1);
  waitingForDecision = false;
}

Object::~Object() {
  if (objectInfo != NULL)
    delete objectInfo;

  for (std::list<Command*>::iterator it = pendingCommandList.begin() ; it != pendingCommandList.end() ; it++)
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

std::list<Command*> Object::getPendingCommands() {
  return pendingCommandList;
}

void Object::enqueueOrUpdate(Command* _cmd) {
  for (std::list<Command*>::iterator it = pendingCommandList.begin() ; it != pendingCommandList.end() ; it++)
    if ((*it)->getId() == _cmd->getId()) {
      delete *it;
      pendingCommandList.erase(it);
      break;
    }

  pendingCommandList.push_back(new Command(_cmd));
  pendingCommandList.sort(Command::compareStampThenId);
}

void Object::tryFlushingCmdQueue(CommandType _type) { // TODO: seria melhor checar se o comando na frente da fila é o mesmo (e nao se o laststamp é o mesmo)
  pendingCommandList.sort(Command::compareStampThenId);
  if (pendingCommandList.empty()) {
    cout << "Object::tryFlushingCmdQueue: command queue of object " << this->getInfo()->getId() << " is CLEAR." << endl;
    return;
  }
  else {
    cout << "Object::tryFlushingCmdQueue: command queue of object " << this->getInfo()->getId() << " has " << (int) pendingCommandList.size() << " elements." << endl;
  }
  Command* nextCmd = new Command(pendingCommandList.front());
  if (nextCmd->getStage() != DELIVERABLE) {
    for (std::list<Command*>::iterator it = pendingCommandList.begin() ; it != pendingCommandList.end() ; it++) {
      cout << "Object::tryFlushingCmdQueue: command " << (*it)->getId() << " has stamp " << (*it)->getStamp();
      if ((*it)->getStage() == DELIVERABLE) cout << " and is DELIVERABLE\n*\n*\n*\n*\n*\n*\n*\n*\n*\n*\n*\n*\n*\n*\n*\n*";
      cout << endl;
    }
    cout << "Object::tryFlushingCmdQueue: first command of object " << this->getInfo()->getId() << " is not deliverable yet." << endl;

    return;
  }
  std::list<ObjectInfo*> targetList = nextCmd->getTargetList();

  bool allObjsReady = true;
  /**
  for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
    Object* obj = Object::getObjectById((*ittarget)->getId());
    if ((*ittarget)->getLastStamp() != obj->getInfo()->getLastStamp()) {
      allObjsReady = false;
      break;
    }
  }
  **/
  if (allObjsReady) {
    cout << "Object::tryFlushingCmdQueue: command delivered to object " << this->getInfo()->getId() << endl;
    for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
      Object* obj = Object::getObjectById((*ittarget)->getId());
      obj->handleConservativeDelivery(nextCmd);
      obj->objectInfo->setLastStamp(nextCmd->getStamp());
      delete obj->pendingCommandList.front();
      obj->pendingCommandList.pop_front();
    }
    for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
      Object* obj = Object::getObjectById((*ittarget)->getId());
      obj->tryFlushingCmdQueue(CONSERVATIVE);
    }
  }
  delete nextCmd;
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
