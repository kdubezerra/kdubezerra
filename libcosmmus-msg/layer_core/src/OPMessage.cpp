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
  // TODO Auto-generated constructor stub
}

OPMessage::~OPMessage() {
  for (std::list<Command*>::iterator it = commandList.begin ; it != commandList.end() ; it++)
    delete *it;

  for (std::list<Object*>::iterator it = stateList.begin() ; it != stateList.end() ; it++)
    delete *it;

  delete extraPayload;
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
    opMsg->addExtraPayload(_msg->getMessage(index++));
  }

  return OPMessage;
}

void OPMessage::setObjectFactory(ObjectFactory* _factory) {
  objectFactory = _factory;
}
