/*
 * Command.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Command.h"
#include "../include/Group.h"
#include "../include/Object.h"
#include "../include/ObjectInfo.h"

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
  commandContent = new Message(_cmd->commandContent);
  withTargets = _cmd->withTargets;
  withGroups = _cmd->withGroups;
  withContent = _cmd->withContent;
  optimistic = _cmd->optimistic;
  conservative = _cmd->conservative;
  for (std::list<Group*>::iterator it = _cmd->groupList.begin() ; it != _cmd->groupList.end() ; it++) {
    Group* groupCopy = new Group(*it);
    groupList.push_back(groupCopy);
  }
  for (std::list<ObjectInfo*>::iterator it = _cmd->targetList.begin() ; it != _cmd->targetList.end() ; it++) {
    ObjectInfo* targetCopy = new ObjectInfo(*it);
    targetList.push_back(targetCopy);
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

bool Command::equals(Command* _other) {

  if (!this->commandContent->equals(_other->commandContent)
      || this->withContent != _other->withContent
      || this->withGroups != _other->withGroups
      || this->withPriorStates != _other->withPriorStates
      || this->withTargets != _other->withTargets
      || this->conservative != _other->conservative
      || this->optimistic != _other->conservative) {
    return false;
  }

  std::list<ObjectInfo*>::iterator oiitthis = this->targetList.begin();
  std::list<ObjectInfo*>::iterator oiitother = _other->targetList.begin();
  while(true) {
    if (oiitthis == this->targetList.end() || oiitother == _other->targetList.end()) {
      if (oiitthis == this->targetList.end() && oiitother == _other->targetList.end())
        break;
      else
        return false;
    }
    if ((*oiitthis)->getId() != (*oiitother)->getId())
      return false;
    oiitthis++;
    oiitother++;
  }

  std::list<Object*>::iterator obitthis = this->necessaryStates.begin();
  std::list<Object*>::iterator obitother = _other->necessaryStates.begin();
  while(true) {
    if (obitthis == this->necessaryStates.end() || obitother == _other->necessaryStates.end()) {
      if (obitthis == this->necessaryStates.end() && obitother == _other->necessaryStates.end())
        break;
      else
        return false;
    }
    if ( !(*obitthis)->equals(*obitother)  )
      return false;
    obitthis++;
    obitother++;
  }

  std::list<Group*>::iterator gitthis = this->groupList.begin();
  std::list<Group*>::iterator gitother = _other->groupList.begin();
  while(true) {
    if (gitthis == this->groupList.end() || gitother == _other->groupList.end()) {
      if (gitthis == this->groupList.end() && gitother == _other->groupList.end())
        break;
      else
        return false;
    }
    if ( (*gitthis)->getId() != (*gitother)->getId() )
      return false;
    gitthis++;
    gitother++;
  }

  return true;
}

void Command::addTarget(ObjectInfo* _obj) {
  targetList.push_back(_obj);
  withTargets = true;
}

void Command::setTargetList(std::list<ObjectInfo*> _targetList) {
  targetList = _targetList;
  withTargets = true;
}


std::list<ObjectInfo*> Command::getTargetList() {
  return targetList;
}

void Command::addGroup(Group* _group) {
  groupList.push_back(_group);
  withGroups = true;
}

void Command::setGroupList(std::list<Group*> _groupList) {
  groupList = _groupList;
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

Message* Command::packToNetwork(Command* _cmd) {
  Message* cmdMsg = new Message();

  cmdMsg->addBool(_cmd->hasContent());
  cmdMsg->addBool(_cmd->knowsTargets());
  cmdMsg->addBool(_cmd->hasPriorStates());
  cmdMsg->addBool(_cmd->knowsGroups());
  cmdMsg->addBool(_cmd->isOptimisticallyDeliverable());
  cmdMsg->addBool(_cmd->isConservativelyDeliverable());

  if (_cmd->hasContent()) cmdMsg->addMessage(new Message(_cmd->getContent()));
  if (_cmd->knowsTargets()) cmdMsg->addMessage(ObjectInfo::packObjectInfoListToNetwork(_cmd->getTargetList()));
  if (_cmd->hasPriorStates()) cmdMsg->addMessage(Object::packObjectListToNetwork(_cmd->getPriorStateList()));
  if (_cmd->knowsGroups()) cmdMsg->addMessage(Group::packGroupListToNetwork(_cmd->getGroupList()));

  return cmdMsg;
}

Command* Command::unpackFromNetwork(Message* _msg) {
  Command* cmd = new Command();
  int msgIndex = 0;

  bool hasContent = _msg->getBool(0);
  cmd->setKnowsTargets(_msg->getBool(1));
  cmd->setKnowsGroups(_msg->getBool(2));
  cmd->setOptimisticallyDeliverable(_msg->getBool(3));
  cmd->setConservativelyDeliverable(_msg->getBool(4));
  if (hasContent) cmd->setContent(_msg->getMessage(msgIndex++));
  if (cmd->knowsTargets()) cmd->setTargetList(ObjectInfo::unpackObjectInfoListFromNetwork(_msg->getMessage(msgIndex++)));
  if (cmd->knowsGroups()) cmd->setGroupList(Group::unpackGroupListFromNetwork(_msg->getMessage(msgIndex)));

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
