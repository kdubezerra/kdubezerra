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
  commandId = logicalStamp = -1;
  timeStamp = 0;
  stage = PROPOSING_LOCAL;
  withTargets = false;
  withGroups = false;
  withPriorStates = false;
  withContent = false;
  optimistic = false;
  conservative = false;
}

Command::Command(long _id) {
  commandId = _id;
  commandContent = NULL;
  commandId = logicalStamp = -1;
  timeStamp = 0;
  stage = PROPOSING_LOCAL;
  withTargets = false;
  withGroups = false;
  withPriorStates = false;
  withContent = false;
  optimistic = false;
  conservative = false;
}

Command::Command(Command* _other) {
  if (_other->commandContent != NULL)
    commandContent = new Message(_other->commandContent);
  else
    commandContent = NULL;
  commandId = _other->commandId;
  logicalStamp = _other->logicalStamp;
  timeStamp = _other->timeStamp;
  stage = _other->stage;
  withTargets = _other->withTargets;
  withGroups = _other->withGroups;
  withPriorStates = _other->withPriorStates;
  withContent = _other->withContent;
  optimistic = _other->optimistic;
  conservative = _other->conservative;
  for (std::list<ObjectInfo*>::iterator it = _other->targetList.begin() ; it != _other->targetList.end() ; it++) {
    targetList.push_back(new ObjectInfo(*it));
  }
  for (std::list<Group*>::iterator it = _other->groupList.begin() ; it != _other->groupList.end() ; it++) {
    groupList.push_back(new Group(*it));
  }
  for (std::list<Object*>::iterator it = _other->priorStateList.begin() ; it!= _other->priorStateList.end() ; it++) {
    priorStateList.push_back(Object::copyObject(*it));
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

  for (std::list<Object*>::iterator it = priorStateList.begin() ; it != priorStateList.end() ; it++)
    if (*it != NULL)
      delete *it;
}

bool Command::equals(Command* other) {

  if (!this->commandContent->equals(other->commandContent)
      || this->commandId != other->commandId
      || this->logicalStamp != other->logicalStamp
      || this->timeStamp != other->timeStamp
      || this->stage != other->stage
      || this->withContent != other->withContent
      || this->withGroups != other->withGroups
      || this->withPriorStates != other->withPriorStates
      || this->withTargets != other->withTargets
      || this->conservative != other->conservative
      || this->optimistic != other->conservative
      || this->targetList.size() != other->targetList.size()
      || this->priorStateList.size() != other->priorStateList.size()
      || this->groupList.size() != other->groupList.size()) {
    return false;
  }


  std::list<ObjectInfo*>::iterator oiitother = other->targetList.begin();
  for (std::list<ObjectInfo*>::iterator itthis = targetList.begin(); itthis != targetList.end() ; itthis++) {
    if ((*itthis)->getId() != (*oiitother)->getId())
      return false;
    oiitother++;
  }


  std::list<Object*>::iterator obitother = other->priorStateList.begin();
  for (std::list<Object*>::iterator itthis = priorStateList.begin(); itthis != priorStateList.end() ; itthis++) {
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


bool Command::compareLogicalStampThenId(Command* c1, Command* c2) {
  if (c1->logicalStamp != c2->logicalStamp)
    return c1->logicalStamp < c2->logicalStamp;
  else
    return c1->commandId < c2->commandId;
}

bool Command::compareTimeStampThenId(Command* c1, Command* c2) {
  if (c1->timeStamp != c2->timeStamp)
    return c1->timeStamp < c2->timeStamp;
  else
    return c1->commandId < c2->commandId;
}

bool Command::compareLSthenTSthenID(Command* c1, Command* c2) {
  if (c1->logicalStamp != c2->logicalStamp)
    return c1->logicalStamp < c2->logicalStamp;
  else if (c1->timeStamp != c2->timeStamp)
    return c1->timeStamp < c2->timeStamp;
  else
    return c1->commandId < c2->commandId;
}

bool Command::compareId(Command* c1, Command* c2) {
  return c1->commandId < c2->commandId;
}

bool Command::hasSameId(Command* c1, Command* c2) {
  return c1->getId() == c1->getId();
}

void Command::removeDuplicates(std::list<Command*>& _cmdlist) {
  _cmdlist.sort(compareId);
  _cmdlist.unique(hasSameId);
}

void Command::addTarget(ObjectInfo* _obj) {
  targetList.push_back(new ObjectInfo(_obj));
  withTargets = true;
}

void Command::addTargetList(std::list<ObjectInfo*> _targetList) {
  for (std::list<ObjectInfo*>::iterator it = _targetList.begin() ; it != _targetList.end() ; it++)
    addTarget(*it);
}

std::list<ObjectInfo*> Command::getTargetList() {
  return targetList;
}

void Command::addPriorState(Object* _state) {
  priorStateList.push_back(Object::copyObject(_state));
  withPriorStates = true;
}

void Command::addPriorStateList(std::list<Object*> _stateList) {
  for (std::list<Object*>::iterator it = _stateList.begin() ; it != _stateList.end() ; it++)
    addPriorState(*it);
}

std::list<Object*> Command::getPriorStateList() {
  return priorStateList;
}

void Command::addGroup(Group* _group) {
  groupList.push_back(new Group(_group));
  withGroups = true;
}

void Command::addGroupList(std::list<Group*> _groupList) {
  for(std::list<Group*>::iterator it = _groupList.begin() ; it != _groupList.end() ; it++)
    addGroup(*it);
}

void Command::findGroups() {
  std::list<Group*> grpSelection;
  if (withGroups)
    return;
  std::list<Group*> allGroups = Group::getGroupsList();
  for (std::list<ObjectInfo*>::iterator ittarget = targetList.begin() ; ittarget != targetList.end() ; ittarget++) {
    for (std::list<Group*>::iterator itgroup = allGroups.begin() ; itgroup != allGroups.end() ; itgroup++) {
      if ((*itgroup)->hasObject(*ittarget)) {
        grpSelection.push_back(*itgroup);
        break;
      }
    }
  }
  grpSelection.sort();
  grpSelection.unique();
  for (std::list<Group*>::iterator it = grpSelection.begin() ; it != grpSelection.end() ; it++)
    groupList.push_back(new Group(*it));
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

void Command::calcLogicalStamp(Group* _localGroup) {
  logicalStamp = -1;

  for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++) {
    Object* obj = Object::getObjectById((*it)->getId());
    if (_localGroup != NULL && !_localGroup->hasObject(*it))
      continue;
    if (obj->getInfo()->getClock() > logicalStamp) {
      logicalStamp = obj->getInfo()->getClock();
    }
  }
}

long Command::getLogicalStamp() {
  return logicalStamp;
}

void Command::setLogicalStamp(long _stamp) {
  logicalStamp = _stamp;
}

long Command::getTimeStamp() {
  return timeStamp;
}

void Command::setTimeStamp(long _stamp) {
  timeStamp = _stamp;
}

CommandStage Command::getStage() {
  return stage;
}

void Command::setStage(CommandStage _stage) {
  stage = _stage;
}

long Command::getId() {
  return commandId;
}

void Command::setId(long _id) {
  commandId = _id;
}

Message* Command::packToNetwork(Command* _cmd) {
  Message* cmdMsg = new Message();

  cmdMsg->addInt((int) _cmd->commandId);
  cmdMsg->addInt((int) _cmd->stage);

  cmdMsg->addLong(_cmd->logicalStamp);
  cmdMsg->addLong(_cmd->timeStamp);

  cmdMsg->addBool(_cmd->hasContent());
  cmdMsg->addBool(_cmd->knowsTargets());
  cmdMsg->addBool(_cmd->hasPriorStates());
  cmdMsg->addBool(_cmd->knowsGroups());
  cmdMsg->addBool(_cmd->isOptimisticallyDeliverable());
  cmdMsg->addBool(_cmd->isConservativelyDeliverable());

  if (_cmd->hasContent())     cmdMsg->addMessage(new Message(_cmd->getContent()));
  if (_cmd->knowsTargets())   cmdMsg->addMessage(ObjectInfo::packListToNetwork(_cmd->getTargetList()));
  if (_cmd->hasPriorStates()) cmdMsg->addMessage(Object::packListToNetwork(_cmd->getPriorStateList()));
  if (_cmd->knowsGroups())    cmdMsg->addMessage(Group::packListToNetwork(_cmd->getGroupList()));

  return cmdMsg;
}

Command* Command::unpackFromNetwork(Message* _msg) {
  Command* cmd = new Command();

  int iint = 0;
  cmd->commandId = _msg->getInt(iint++);
  cmd->stage = (CommandStage) _msg->getInt(iint++);

  int ilong = 0;
  cmd->logicalStamp = _msg->getLong(ilong++);
  cmd->timeStamp = _msg->getLong(ilong++);

  int ibool = 0;
  cmd->withContent     = _msg->getBool(ibool++);
  cmd->withTargets     = _msg->getBool(ibool++);
  cmd->withPriorStates = _msg->getBool(ibool++);
  cmd->withGroups      = _msg->getBool(ibool++);
  cmd->optimistic      = _msg->getBool(ibool++);
  cmd->conservative    = _msg->getBool(ibool++);

  int imsg = 0;
  if (cmd->withContent)     cmd->commandContent = new Message(_msg->getMessage(imsg++));
  if (cmd->withTargets)     cmd->targetList = ObjectInfo::unpackListFromNetwork(_msg->getMessage(imsg++));
  if (cmd->withPriorStates) cmd->priorStateList = Object::unpackListFromNetwork(_msg->getMessage(imsg++));
  if (cmd->withGroups)      cmd->groupList = Group::unpackListFromNetwork(_msg->getMessage(imsg));

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

  int cmdCount = _msg->getInt(0);
  for (int i = 0 ; i < cmdCount ; i++) {
    cmdList.push_back(Command::unpackFromNetwork(_msg->getMessage(i)));
  }

  return cmdList;
}
