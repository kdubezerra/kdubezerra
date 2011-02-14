/*
 * Client.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

// core level classes
#include "../include/Client.h"
#include "../include/Object.h"
#include "../include/OPMessage.h"

// network level classes
#include "../../layer_network/include/FIFOReliableClient.h"

using namespace optpaxos;
using namespace netwrapper;

Client::Client() {
  FIFOReliableClient* netClient = new FIFOReliableClient();
  netClient->setCallbackInterface(this);
  callbackClient = NULL;
}

Client::~Client() {
  // TODO Auto-generated destructor stub
}

int Client::connect(std::string _address, unsigned _port) {
  return netClient->connect(_address, _port);
}

int Client::disconnect() {
  return netClient->disconnect();
}

void Client::submitCommand(Command* _cmd) {
  OPMessage* cmdOpMsg = new OPMessage();
  cmdOpMsg->setType(CLIENT_CMD);
  cmdOpMsg->addCommand(_cmd);
  Message* packedCmdOpMsg = OPMessage::packToNetwork(cmdOpMsg);
  netClient->sendMessage(packedCmdOpMsg);
  delete packedCmdOpMsg;
  delete cmdOpMsg;
}

void Client::submitApplicationMessage(netwrapper::Message* _msg) {
  OPMessage* opMsg = new OPMessage();
  opMsg->setType(APP_MSG);
  opMsg->setExtraPayload(_msg);
  Message* msgOpMsg = OPMessage::packToNetwork(opMsg);
  netClient->sendMessage(msgOpMsg);
  delete msgOpMsg;
  delete opMsg;
}

void Client::setCallbackInterface(optpaxos::ClientInterface* _callbackClient) {
  callbackClient = _callbackClient;
}

optpaxos::ClientInterface* Client::getCallbackClient() {
  return callbackClient;
}

void Client::handleServerMessage(netwrapper::Message* _msg) {
  OPMessage* opMsg = OPMessage::unpackFromNetwork(_msg);
  handleOPMessage(opMsg);
  delete opMsg;
}

void Client::handleOPMessage(OPMessage* _opMsg) {
  if (_opMsg->hasState()) {
    std::list<Object*> states = _opMsg->getStateList();
    for (std::list<Object*>::iterator it = states.begin() ; it != states.end() ; it++) {
      Object::handleStateUpdate(*it);
    }
  }

  if (_opMsg->hasCommand()) {
    std::list<Command*> coms = _opMsg->getCommandList();
    for (std::list<Command*>::iterator it = coms.begin() ; it != coms.end() ; it++) {
      Object::handleCommand(*it);
    }
  }

  if (_opMsg->hasExtraPayload()) {
    callbackClient->handleMessage(_opMsg->getExtraPayload());
  }
}

void handleStateUpdate(Object* _state) {
/* TODO: create a coherent model of state and its updates: when the server sends an update, what
 *       is it updating? There at least two and, possibly, one more state delivery queue. How is
 *       this going to work? When will one state overwrite the other? Every object should have a
 *       command queue and the library is responsible for controlling this? Maybe this should be
 *       delegated do the application? Although this is not critical for the development of this
 *       library, it would be interesting to have this problem worked out.
 *
 * UPDATE: for now, the "visible" state will be the optimistic one, which will be overwritten and
 *     recalculated once a discrepancy between the conservative delivery order and the previously
 *     considered conservative one.
 */

}
