/*
 * Command.cpp
 *
 *  Created on: 31/01/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Command.h"

Command::Command() {
  // TODO Auto-generated constructor stub
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

  for (std::list<GameServer*>::iterator it = serverList.begin() ; it != serverList.end() ; it++)
    if (*it != NULL)
      delete *it;
}

void Command::addTarget(Object* _obj) {
  targetList.push_back(_obj);
}

std::list<Object*> Command::getTargetList() {
  return targetList;
}

void Command::addServer(GameServer* _server) {
  serverList.push_back(_server);
}

std::list<GameServer*> Command::getServerList() {
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
