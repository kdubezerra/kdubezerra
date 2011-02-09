/*
 * Client.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

// core level classes
#include "../include/Client.h"
#include "../include/OPMessage.h"

// network level classes
#include "../../layer_network/include/FIFOReliableClient.h"

using namespace optpaxos;
using namespace netwrapper;

Client::Client() {
  FIFOReliableClient* netClient = new FIFOReliableClient();
  netClient->setCallbackInterface(this);
  objectFactory = NULL;
  callbackClient = NULL;
}

Client::~Client() {
  // TODO Auto-generated destructor stub
}

int Client::connect(string _address) {
  return netClient->connect(_address);
}

int Client::disconnect() {
  return netClient->disconnect();
}

void Client::submitCommand(Command* _cmd) {
  Message* cmdMsg = Command::packToNetwork();
  netClient->sendMessage(cmdMessage);
  delete cmdMsg;
}

void Client::submitApplicationMessage(netwrapper::Message* _msg) {
  OPMessage* opMsg = new OPMessage();
  opMsg->setType(APP_MSG);
  opMsg->addExtraPayload(_msg);
  Message* msgOpMsg = OPMessage::packToNetwork(opMsg);
  netClient->sendMessage(msgOpMsg);
  delete msgOpMsg;
  delete opMsg;
}

void Client::setObjectFactory(ObjectFactory* _factory) {
  ObjectFactory = _factory;
  Command::setObjectFactory(_factory);
  OPMessage::setObjectFactory(_factory);
}

void Client::setCallbackInterface(optpaxos::ClientInterface* _callbackClient) {
  callbackClient = _callbackClient;
}

optpaxos::ClientInterface* Client::getCallbackClient() {
  return callbackClient;
}

void Client::handleMessage(netwrapper::Message* _msg) {
  OPMessage* opMsg = OPMessage::unpackFromNetwork(_msg);
  handleOPMessage(opMsg);
  delete opMsg;
}

void Client::handleOPMessage(OPMessage* _opMsg) {
  if (_opMsg->hasState()) {
    list<Object*> states = _opMsg->getStateList();
    for (list<Object*>::iterator it = states.begin() ; it != states.end() ; it++) {
      handleStateUpdate(*it);
    }
  }

  if (_opMsg->hasCommand()) {
    list<Command*> coms = _opMsg->getCommandList();
    for (list<Command*>::iterator it = coms.begin() ; it != coms.end() ; it++) {
      handleCommand(*it);
    }
  }

  if (_opMsg->hasExtraPayload()) {
    callbackClient->handleMessage(_opMsg->getExtraPayload());
  }
}

/*!
 * \brief This method is responsible for listing the objects affected by the command. Also,
 * it checks whether the command's delivery is either optimistic or conservative, and delivers
 * to the correspondent application level queue of each object. If the client doesn't have an
 * up-to-date state of all objects affected by the command, this method also forwards the
 * newest state (prior to the execution of the command) to each object needing update.
 * \param _cmd The core layer level command to be parsed.
 */
void Client::handleCommand(Command* _cmd) {
  list<Object*> targets = _cmd->getTargetList();

  for (list<Object*>::iterator it = targets.begin() ; it != targets.end() ; it++) {
    Object* obj = Object::getObjectById((*it)->getId());
    if (_cmd->isOptimisticallyDeliverable())
      obj->handleNewOptimisticState(*it);
    if (_cmd->isConservativelyDeliverable())
      obj->handleNewConservativeState(*it);
  }

  for (list<Object*>::iterator it = targets.begin() ; it != targets.end() ; it++) {
    Object* obj = Object::getObjectById((*it)->getId());
    if (_cmd->isOptimisticallyDeliverable())
      obj->handleOptimisticDelivery(_cmd->getContent());
    if (_cmd->isConservativelyDeliverable())
      obj->handleConservativeDelivery(_cmd->getContent());
      // TODO: at this point, the delivered command must be checked against the first one in the optimistic queue.
      //       Should be found a difference, action must be taken to correct this discrepancy.
      // default: application handles it.
  }
}

void handleStateUpdate(GameObject* _state) {
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
