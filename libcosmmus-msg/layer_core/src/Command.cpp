/*
 * Command.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Command.h"

using namespace optpaxos;

Command::Command() {
  commandContent = NULL;
  withTargets = false;
  optimistic = false;
  conservative = false;
}

Command::~Command() {
  if (commandContent != NULL)
    delete commandContent;

  for (std::list<Object*>::iterator it = targetList.begin() ; it != targetList.end() ; it++)
    if (*it != NULL)
     delete *it;

  for (std::list<Server*>::iterator it = serverList.begin() ; it != serverList.end() ; it++)
    if (*it != NULL)
      delete *it;
}

void Command::addTarget(Object* _obj) {
  targetList.push_back(_obj);
}

void Command::setTargetList(std::list<Object*> _targetList) {
  targetList = _targetList;
}


std::list<Object*> Command::getTargetList() {
  return targetList;
}

void Command::addServer(Server* _server) {
  serverList.push_back(_server);
}

std::list<Server*> Command::getServerList() {
  return serverList;
}

void Command::setContent(Message* _content) {
  if (commandContent != NULL)
    delete commandContent;
  commandContent = _content;
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
