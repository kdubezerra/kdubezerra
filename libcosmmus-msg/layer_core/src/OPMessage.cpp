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
  messageType = (OPMessageType) 0;
}

OPMessage::OPMessage(OPMessage* _other) {
  messageType = _other->messageType;
  addCommandList(_other->commandList);
  addMessageCopyList(_other->messageList);
  addStateList(_other->stateList);
}

OPMessage::~OPMessage() {
  for (std::list<Command*>::iterator it = commandList.begin() ; it != commandList.end() ; it++)
    delete *it;

  for (std::list<Object*>::iterator it = stateList.begin() ; it != stateList.end() ; it++)
    delete *it;

  for (std::list<Message*>::iterator it = messageList.begin() ; it != messageList.end() ; it++)
    delete *it;
}

bool OPMessage::equals (OPMessage* other) {
  if (this->messageType != other->messageType
      || this->commandList.size() != other->commandList.size()
      || this->stateList.size() != other->stateList.size()
      || this->messageList.size() != other->messageList.size())
    return false;

  std::list<Command*>::iterator icother = other->commandList.begin();
  for (std::list<Command*>::iterator itthis = commandList.begin() ; itthis != commandList.end() ; itthis++) {
    if (!(*itthis)->equals(*icother))
      return false;
    icother++;
  }

  std::list<Object*>::iterator ioother = other->stateList.begin();
  for (std::list<Object*>::iterator itthis = stateList.begin() ; itthis != stateList.end() ; itthis++) {
    if (!(*itthis)->equals(*ioother))
      return false;
    ioother++;
  }

  std::list<Message*>::iterator imother = other->messageList.begin();
  for (std::list<Message*>::iterator itthis = messageList.begin() ; itthis != messageList.end() ; itthis++) {
    if (!(*itthis)->equals(*imother))
      return false;
    imother++;
  }

  return true;
}

void OPMessage::setType(OPMessageType _msgType) {
  messageType = _msgType;
}

void OPMessage::addCommand(Command* _cmd) {
  commandList.push_back(_cmd);
}

void OPMessage::addCommandList(std::list<Command*> _cmdList) {
  for (std::list<Command*>::iterator it = _cmdList.begin() ; it != _cmdList.end() ; it++)
    addCommand(*it);
}

void OPMessage::addState(Object* _state) {
  stateList.push_back(Object::copyObject(_state));
}

void OPMessage::addStateList(std::list<Object*> _stateList) {
  for (std::list<Object*>::iterator it = _stateList.begin() ; it != _stateList.end() ; it++)
    addState(*it);
}

void OPMessage::addMessage(Message* _msg) {
  messageList.push_back(_msg);
}

void OPMessage::addMessageCopy(Message* _msg) {
  messageList.push_back(new Message(_msg));
}

void OPMessage::addMessageList(std::list<Message*> _msgList) {
  for (std::list<Message*>::iterator it = _msgList.begin() ; it != _msgList.end() ; it++)
    addMessage(*it);
}

void OPMessage::addMessageCopyList(std::list<Message*> _msgList) {
  for (std::list<Message*>::iterator it = _msgList.begin() ; it != _msgList.end() ; it++)
    addMessageCopy(*it);
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
  return commandList.empty() == false;
}

bool OPMessage::hasState() {
  return stateList.empty() == false;
}

bool OPMessage::hasMessage() {
  return messageList.empty() == false;
}

Message* OPMessage::packToNetwork(OPMessage* _opMsg) {
  Message* packedOpMsg = new Message();

  packedOpMsg->addInt((int) _opMsg->messageType);

  packedOpMsg->addBool(_opMsg->hasCommand());
  packedOpMsg->addBool(_opMsg->hasState());
  packedOpMsg->addBool(_opMsg->hasMessage());

  if (_opMsg->hasCommand()) {
    packedOpMsg->addMessage(Command::packCommandListToNetwork(_opMsg->commandList));
  }

  if (_opMsg->hasState()) {
    packedOpMsg->addMessage(Object::packListToNetwork(_opMsg->stateList));
  }

  if (_opMsg->hasMessage()) {
    packedOpMsg->addInt((int) _opMsg->messageList.size());
    for (std::list<Message*>::iterator it = _opMsg->messageList.begin() ; it != _opMsg->messageList.end() ; it++)
      packedOpMsg->addMessage(new Message(*it));
  }

  return packedOpMsg;
}

OPMessage* OPMessage::unpackFromNetwork(netwrapper::Message* _msg) {
  OPMessage* opMsg = new OPMessage();

  opMsg->setType((OPMessageType) _msg->getInt(0));

  bool hasCmd = _msg->getBool(0);
  bool hasState = _msg->getBool(1);
  bool hasMsg = _msg->getBool(2);

  int index = 0;

  if (hasCmd) {
    opMsg->addCommandList(Command::unpackCommandListFromNetwork(_msg->getMessage(index++)));
  }
  if (hasState) {
    opMsg->addStateList(Object::unpackListFromNetwork(_msg->getMessage(index++)));
  }
  if (hasMsg) {
    int messageCount = _msg->getInt(1);
    for (int j = index ; j < index + messageCount ; j++)
      opMsg->messageList.push_back(new Message(_msg->getMessage(j)));
  }

  return opMsg;
}
