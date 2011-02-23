/*
 * Object.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Command.h"
#include "../include/Object.h"
#include "../include/ObjectFactory.h"
#include "../include/ObjectInfo.h"

using namespace optpaxos;
using namespace netwrapper;

std::map<int, Object*> Object::objectIndex;
ObjectFactory* Object::objectFactory = NULL;

Object::Object() {
  objectInfo = new ObjectInfo();
  waitingForDecision = false;
}

Object::Object(Object* _other) {
  objectInfo = new ObjectInfo(_other->objectInfo);
}

Object::Object(int _id) {
  objectInfo = new ObjectInfo();
  objectInfo->setId(_id);
}

Object::~Object() {
  delete objectInfo;
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
  objectInfo = _objInfo;
}

Object* Object::getObjectById(int _id) {
  std::map<int, Object*>::iterator finder = objectIndex.find(_id);
  if (finder != objectIndex.end())
    return finder->second;
  else
    return NULL;
}

void Object::setObjectFactory(ObjectFactory* _factory) {
  objectFactory = _factory;
}

ObjectFactory* Object::getObjectFactory() {
  return objectFactory;
}

void Object::enqueue(Command* _cmd, CommandType _type) {
  pendingCommands.push_back(new Command(_cmd));
  pendingCommands.sort(Command::compareLT);
}

void Object::tryFlushingCmdQueue(CommandType _type) {
  bool allObjsReady = true;
  if (pendingCommands.empty())
    return;
  Command* nextCmd = new Command(pendingCommands.front());
  std::list<ObjectInfo*> targetList = nextCmd->getTargetList();
  for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
    Object* obj = Object::getObjectById((*ittarget)->getId());
    if ((*ittarget)->getLastStamp() != obj->getInfo()->getLastStamp())
      allObjsReady = false;
  }
  if (allObjsReady) {
    for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
      Object* obj = Object::getObjectById((*ittarget)->getId());
      obj->handleConservativeDelivery(nextCmd);
      delete obj->pendingCommands.front();
      obj->pendingCommands.pop_front();
    }
    for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
      Object* obj = Object::getObjectById((*ittarget)->getId());
      obj->tryFlushingCmdQueue(CONSERVATIVE);
    }
  }
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

Message* Object::packToNetwork(Object* _obj) {
  Message* msg = new Message();
  msg->addMessage(ObjectInfo::packToNetwork(_obj->objectInfo));
  msg->addMessage(objectFactory->packToNetwork(_obj));
  return msg;
}

Message* Object::packListToNetwork(std::list<Object*> _objList) {
  Message* objListMsg = new Message();

  objListMsg->addInt((int) _objList.size());
  for (std::list<Object*>::iterator it = _objList.begin() ; it != _objList.end() ; it++)
    objListMsg->addMessage(Object::packToNetwork(*it));

  return objListMsg;
}

Object* Object::unpackFromNetwork(netwrapper::Message* _msg) {
  Object* obj = objectFactory->unpackFromNetwork(_msg->getMessage(0));
  obj->setInfo(ObjectInfo::unpackFromNetwork(_msg->getMessage(1)));
  return obj;
}

std::list<Object*> Object::unpackListFromNetwork(netwrapper::Message* _msg) {
  std::list<Object*> objList;

  int objectCount = _msg->getInt(0);
  for (int index = 0 ; index < objectCount ; index++)
    objList.push_back(Object::unpackFromNetwork(_msg->getMessage(index)));

  return objList;
}
