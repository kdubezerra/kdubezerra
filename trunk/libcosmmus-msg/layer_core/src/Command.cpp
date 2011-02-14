/*
 * Command.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Command.h"

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
  for (std::list<Object*>::iterator it = _cmd->targetList.begin() ; it != _cmd->targetList.end() ; it++) {
    Object* targetCopy = new Object(*it);
    targetList.push_back(targetCopy);
  }
}

Command::~Command() {
  if (commandContent != NULL)
    delete commandContent;

  for (std::list<Object*>::iterator it = targetList.begin() ; it != targetList.end() ; it++)
    if (*it != NULL)
     delete *it;

  for (std::list<Group*>::iterator it = groupList.begin() ; it != groupList.end() ; it++)
    if (*it != NULL)
      delete *it;
}

void Command::addTarget(Object* _obj) {
  targetList.push_back(_obj);
  withTargets = true;
}

void Command::setTargetList(std::list<Object*> _targetList) {
  targetList = _targetList;
  withTargets = true;
}


std::list<Object*> Command::getTargetList() {
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

  if (_cmd->hasContent()) cmdMsg->addMessageCopy(_cmd->getContent());
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
  if (cmd->knowsTargets()) cmd->setTargetList(Command::unpackCommandListFromNetwork(_msg->getMessage(msgIndex++)));
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
