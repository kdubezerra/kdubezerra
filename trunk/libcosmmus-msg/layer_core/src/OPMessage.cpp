/*
 * OPMessage.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Command.h"
#include "../include/Object.h"
#include "../include/OPMessage.h"

using namespace optpaxos;
using namespace netwrapper;

OPMessage::OPMessage() {
}

OPMessage::~OPMessage() {
  for (std::list<Command*>::iterator it = commandList.begin() ; it != commandList.end() ; it++)
    delete *it;

  for (std::list<Object*>::iterator it = stateList.begin() ; it != stateList.end() ; it++)
    delete *it;

  for (std::list<Message*>::iterator it = messageList.begin() ; it != messageList.end() ; it++)
    delete *it;
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

void OPMessage::addMessage(netwrapper::Message* _msg) {
  messageList.push_back(_msg);
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

std::list<Message*> OPMessage::getMessageList() {
  return messageList;
}

bool OPMessage::hasCommand() {
  return commandList.empty();
}

bool OPMessage::hasState() {
  return stateList.empty();
}

bool OPMessage::hasMessage() {
  return messageList.empty();
}

Message* OPMessage::packToNetwork(OPMessage* _opMsg) {
    Message* netMsg = new Message();

    netMsg->addInt(_opMsg->getType());
    netMsg->addInt((int) _opMsg->messageList.size());

    netMsg->addBool(_opMsg->hasCommand());
    netMsg->addBool(_opMsg->hasState());
    netMsg->addBool(_opMsg->hasMessage());

    if (_opMsg->hasCommand()) {
      netMsg->addMessage(Command::packCommandListToNetwork(_opMsg->getCommandList()));
    }
    if (_opMsg->hasState()) {
      netMsg->addMessage(Object::packObjectListToNetwork(_opMsg->getStateList()));
    }
    if (_opMsg->hasMessage()) {
      std::list<Message*> messageList = _opMsg->messageList;
      for (std::list<Message*>::iterator it = messageList.begin() ; it != messageList.end() ; it++)
        netMsg->addMessage(new Message(*it));
    }

    return netMsg;
  }

OPMessage* OPMessage::unpackFromNetwork(netwrapper::Message* _msg) {
  OPMessage* opMsg = new OPMessage();

  bool hasCmd = _msg->getBool(0);
  bool hasState = _msg->getBool(1);
  bool hasMsg = _msg->getBool(2);

  int index = 0;

  opMsg->setType((OPMessageType) _msg->getInt(0));
  if (hasCmd) {
    opMsg->setCommandList(Command::unpackCommandListFromNetwork(_msg->getMessage(index++)));
  }
  if (hasState) {
    opMsg->setStateList(Object::unpackObjectListFromNetwork(_msg->getMessage(index++)));
  }
  if (hasMsg) {
    int messageCount = _msg->getInt(1);
    for (int j = index ; j < index + messageCount ; j++)
      opMsg->messageList.push_back(new Message(_msg->getMessage(j)));
  }

  return opMsg;
}
