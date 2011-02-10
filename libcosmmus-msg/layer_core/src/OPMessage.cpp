/*
 * OPMessage.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/OPMessage.h"

using namespace optpaxos;
using namespace netwrapper;

OPMessage::OPMessage() {
  extraPayload = NULL;
}

OPMessage::~OPMessage() {
  for (std::list<Command*>::iterator it = commandList.begin ; it != commandList.end() ; it++)
    delete *it;

  for (std::list<Object*>::iterator it = stateList.begin() ; it != stateList.end() ; it++)
    delete *it;

  if (extraPayload != NULL)
    delete extraPayload;
}

void OPMessage::setType(OPMessageType _msgType) {
  messageType = _msgType;
}

void OPMessage::addCommand(Command* _cmd) {
  commandList.push_back(_cmd);
}

void OPMessage::setCommandList(std::list<Command*> _cmdList) {
  commandList = _cmdList;
}

void OPMessage::addState(Object* _state) {
  stateList.push_back(_state);
}

void OPMessage::setStateList(std::list<Object*> _stateList) {
  stateList = _stateList;
}

void OPMessage::setExtraPayload(netwrapper::Message* _payload) {
  extraPayload = _payload;
}

OPMessageType OPMessage::getType() {
  return messageType;
}

std::list<Command*> OPMessage::getCommandList() {
  return commandList;
}

std::list<Object*> OPMessage::getStateList() {
  return stateList;
}

netwrapper::Message* OPMessage::getExtraPayload() {
  return extraPayload;
}

bool OPMessage::hasCommand() {
  return commandList.empty();
}

bool OPMessage::hasState() {
  return stateList.empty();
}

bool OPMessage::hasExtraPayload() {
  if (extraPayload != NULL)
    return true;
  else
    return false;
}

Message* OPMessage::packToNetwork(OPMessage* _opMsg) {
    Message* netMsg = new Message();

    netMsg->addInt(_opMsg->getType());

    netMsg->addBool(_opMsg->hasCommand());
    netMsg->addBool(_opMsg->hasState());
    netMsg->addBool(_opMsg->hasExtraPayload());

    if (_opMsg->hasCommand()) {
      netMsg->addMessage(Command::packCommandListToNetwork(_opMsg->getCommandList()));
    }
    if (_opMsg->hasState()) {
      netMsg->addMessage(Object::packObjectListToNetwork(_opMsg->getStateList()));
    }
    if (_opMsg->hasExtraPayload()) {
      netMsg->addMessage(_opMsg->getExtraPayload());
    }

    return netMsg;
  }

OPMessage* OPMessage::unpackFromNetwork(netwrapper::Message* _msg) {
  OPMessage* opMsg = new OPMessage();

  bool hasCmd = _msg->getBool(0);
  bool hasState = _msg->getBool(1);
  bool hasExtraPayload = _msg->getBool(2);

  int index = 0;

  opMsg->setType(_msg->getInt(0));
  if (hasCmd) {
    opMsg->setCommandList(Command::unpackCommandListFromNetwork(_msg->getMessage(index++)));
  }
  if (hasState) {
    opMsg->setStateList(Object::unpackObjectListFromNetwork(_msg->getMessage(index++)));
  }
  if (hasExtraPayload) {
    opMsg->setExtraPayload(_msg->getMessage(index++));
  }

  return OPMessage;
}
