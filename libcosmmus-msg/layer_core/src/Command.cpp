/*
 * Command.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>

#include "../include/Command.h"
#include "../include/Group.h"
#include "../include/Object.h"
#include "../include/ObjectInfo.h"

using namespace std;
using namespace optpaxos;
using namespace netwrapper;

Command::Command() {
  commandContent = NULL;
  withTargets = false;
  withGroups = false;
  withContent = false;
  optimistic = false;
  conservative = false;
}

Command::Command(Command* _cmd) {
  if (_cmd->commandContent != NULL)
    commandContent = new Message(_cmd->commandContent);
  else
    commandContent = NULL;
  withTargets = _cmd->withTargets;
  withGroups = _cmd->withGroups;
  withContent = _cmd->withContent;
  optimistic = _cmd->optimistic;
  conservative = _cmd->conservative;
  for (std::list<Group*>::iterator it = _cmd->groupList.begin() ; it != _cmd->groupList.end() ; it++) {
    groupList.push_back(new Group(*it));
  }
  for (std::list<ObjectInfo*>::iterator it = _cmd->targetList.begin() ; it != _cmd->targetList.end() ; it++) {
    targetList.push_back(new ObjectInfo(*it));
  }
}

Command::~Command() {
  if (commandContent != NULL)
    delete commandContent;

  for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++)
    if (*it != NULL)
     delete *it;

  for (std::list<Group*>::iterator it = groupList.begin() ; it != groupList.end() ; it++)
    if (*it != NULL)
      delete *it;
}

bool Command::equals(Command* other) {

  if (!this->commandContent->equals(other->commandContent)
      || this->withContent != other->withContent
      || this->withGroups != other->withGroups
      || this->withPriorStates != other->withPriorStates
      || this->withTargets != other->withTargets
      || this->conservative != other->conservative
      || this->optimistic != other->conservative
      || this->targetList.size() != other->targetList.size()
      || this->necessaryStates.size() != other->necessaryStates.size()
      || this->groupList.size() != other->groupList.size()) {
    return false;
  }


  std::list<ObjectInfo*>::iterator oiitother = other->targetList.begin();
  for (std::list<ObjectInfo*>::iterator itthis = targetList.begin(); itthis != targetList.end() ; itthis++) {
    if ((*itthis)->getId() != (*oiitother)->getId())
      return false;
    oiitother++;
  }


  std::list<Object*>::iterator obitother = other->necessaryStates.begin();
  for (std::list<Object*>::iterator itthis = necessaryStates.begin(); itthis != necessaryStates.end() ; itthis++) {
    if (!(*itthis)->equals(*obitother))
      return false;
    obitother++;
  }

  std::list<Group*>::iterator gitother = other->groupList.begin();
    for (std::list<Group*>::iterator itthis = groupList.begin(); itthis != groupList.end() ; itthis++) {
    if ((*itthis)->getId() != (*gitother)->getId())
      return false;
    gitother++;
  }

  return true;
}

bool Command::compareLT(Command* c1, Command* c2) {
  return c1->stamp < c2->stamp;
}

void Command::addTarget(ObjectInfo* _obj) {
  targetList.push_back(new ObjectInfo(_obj));
  withTargets = true;
}

void Command::setTargetList(std::list<ObjectInfo*> _targetList) {
  targetList.clear();
  for (std::list<ObjectInfo*>::iterator it = _targetList.begin() ; it != _targetList.end() ; it++)
    targetList.push_back(new ObjectInfo(*it));
  withTargets = true;
}


std::list<ObjectInfo*> Command::getTargetList() {
  return targetList;
}

void Command::addPriorStateState(Object* _state) {
  necessaryStates.push_back(_state);
}

void Command::setPriorStateList(std::list<Object*> _stateList) {
  necessaryStates = _stateList;
}

std::list<Object*> Command::getPriorStateList() {
  return necessaryStates;
}

void Command::addGroup(Group* _group) {
  groupList.push_back(_group);
  withGroups = true;
}

void Command::setGroupList(std::list<Group*> _groupList) {
  groupList = _groupList;
  withGroups = true;
}

void Command::findGroups() {
  std::list<Group*> allGroups = Group::getGroupsList();
  for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
    for (std::list<Group*>::iterator itgroup = allGroups.begin() ; itgroup != allGroups.end() ; itgroup++) {
      if ((*itgroup)->hasObject(*ittarget)) {
        groupList.push_back(new Group(*itgroup));
        break;
      }
    }
  }
  withGroups = true;
}

std::list<Group*> Command::getGroupList() {
  return groupList;
}

void Command::setContent(Message* _content) {
  if (commandContent != NULL)
    delete commandContent;
  commandContent = _content;
  withContent = true;
}

Message* Command::getContent() {
  return commandContent;
}

void Command::setKnowsTargets(bool _knowsTargets) {
  withTargets = _knowsTargets;
}

bool Command::knowsTargets() {
  return withTargets;
}

void Command::setHasPriorStates(bool _hasPriorStates) {
  withPriorStates = _hasPriorStates;
}

bool Command::hasPriorStates() {
  return withPriorStates;
}

void Command::setKnowsGroups(bool _knowsGroups) {
  withGroups = _knowsGroups;
}

bool Command::knowsGroups() {
  return withGroups;
}

bool Command::hasContent() {
  return withContent;
}

void Command::setOptimisticallyDeliverable(bool _isOpDeliverable) {
  optimistic = _isOpDeliverable;
}

bool Command::isOptimisticallyDeliverable() {
   return optimistic;
}

void Command::setConservativelyDeliverable(bool _isConsDeliverable) {
  conservative = _isConsDeliverable;
}

bool Command::isConservativelyDeliverable() {
  return conservative;
}

void Command::calculateStamp() {
  long greatestStamp = -1;

  for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++) {
    Object* obj = Object::getObjectById((*it)->getId());
    if (obj == NULL) continue;
    if (obj->getInfo()->getLastStamp() > greatestStamp) {
      greatestStamp = obj->getInfo()->getLastStamp();
    }
  }

  if (greatestStamp > this->stamp)
    this->stamp = greatestStamp;
}

long Command::getStamp() {
  return stamp;
}

void Command::setStamp(long _stamp) {
  stamp = _stamp;
}

Message* Command::packToNetwork(Command* _cmd) {
  Message* cmdMsg = new Message();

  cmdMsg->addBool(_cmd->hasContent());
  cmdMsg->addBool(_cmd->knowsTargets());
  cmdMsg->addBool(_cmd->hasPriorStates());
  cmdMsg->addBool(_cmd->knowsGroups());
  cmdMsg->addBool(_cmd->isOptimisticallyDeliverable());
  cmdMsg->addBool(_cmd->isConservativelyDeliverable());

  if (_cmd->hasContent()) cmdMsg->addMessage(new Message(_cmd->getContent()));
  if (_cmd->knowsTargets()) cmdMsg->addMessage(ObjectInfo::packListToNetwork(_cmd->getTargetList()));
  if (_cmd->hasPriorStates()) cmdMsg->addMessage(Object::packListToNetwork(_cmd->getPriorStateList()));
  if (_cmd->knowsGroups()) cmdMsg->addMessage(Group::packListToNetwork(_cmd->getGroupList()));

  return cmdMsg;
}

Command* Command::unpackFromNetwork(Message* _msg) {
  Command* cmd = new Command();
  int msgIndex = 0;

  bool hasContent = _msg->getBool(0);
  cmd->withTargets = _msg->getBool(1);
  cmd->withPriorStates = _msg->getBool(2);
  cmd->withGroups = _msg->getBool(3);
  cmd->optimistic = _msg->getBool(4);
  cmd->conservative = _msg->getBool(5);

  if (hasContent) cmd->commandContent = new Message(_msg->getMessage(msgIndex++));
  if (cmd->withTargets) cmd->setTargetList(ObjectInfo::unpackListFromNetwork(_msg->getMessage(msgIndex++)));
  if (cmd->withPriorStates) cmd->setPriorStateList(Object::unpackListFromNetwork(_msg->getMessage(msgIndex++)));
  if (cmd->withGroups) cmd->setGroupList(Group::unpackListFromNetwork(_msg->getMessage(msgIndex)));

  return cmd;
}

Message* Command::packCommandListToNetwork(std::list<Command*> _cmdList) {
  Message* cmdListMsg = new Message();

  cmdListMsg->addInt((int) _cmdList.size());
  for (std::list<Command*>::iterator it = _cmdList.begin() ; it != _cmdList.end() ; it++) {
    cmdListMsg->addMessage(Command::packToNetwork(*it));
  }

  return cmdListMsg;
}

std::list<Command*> Command::unpackCommandListFromNetwork(Message* _msg) {
  std::list<Command*> cmdList;

  for (int i = 0 ; i < _msg->getInt(0) ; i++) {
    cmdList.push_back(Command::unpackFromNetwork(_msg->getMessage(i)));
  }

  return cmdList;
}
