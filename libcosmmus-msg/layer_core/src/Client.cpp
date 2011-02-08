/*
 * Client.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

// core level classes
#include "../include/Client.h"
#include "../include/ServerMessage.h"

// network level classes
#include "../../layer_network/include/FIFOReliableClient.h"

using namespace optpaxos;
using namespace netwrapper;

Client::Client() {
  FIFOReliableClient* netClient = new FIFOReliableClient();
  netClient->setCallbackInterface(this);
  objModel = NULL;
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
  Message* cmdMsg = new Message();

  if (_cmd->knowsTargets()) {
    cmdMsg->addBool(true);
    std::list<Object*> targets = _cmd->getTargetList();
    for (std::list<Object*>::iterator it = targets.begin() ; it != targets.end() ; it++) {
      cmdMsg->addInt((*it)->getId()); //object's id
    }
  }
  else {
    cmdMsg->addBool(false);
    std::list<Server*> servers = _cmd->getServerList();
    for (std::list<Server*>::iterator it = servers.begin() ; it != servers.end() ; it++) {
      cmdMsg->addInt((*it)->getId()); //server's id
    }
  }

  cmdMsg->addMessage(_cmd->getContent());

  netClient->sendMessage(cmdMessage);
}

void Client::handleMessage(netwrapper::Message* _msg) {
  ServerMessage* serverMsg = getServerMessageFromMsg(_msg);
  handleServerMessage(serverMsg);
  delete serverMsg;
}

void Client::handleServerMessage(ServerMessage* _serverMsg) {
  if (_serverMsg->hasState()) {
    list<Object*> states = _serverMsg->getStateList();
    for (list<Object*>::iterator it = states.begin() ; it != states.end() ; it++) {
      handleStateUpdate(*it);
    }
  }

  if (_serverMsg->hasCommand()) {
    list<Command*> coms = _serverMsg->getCommandList();
    for (list<Command*>::iterator it = coms.begin() ; it != coms.end() ; it++) {
      handleCommand(*it);
    }
  }

  if (_serverMsg->hasExtraPayload()) {
    callbackInterface->handleServerMessage(_serverMsg->getExtraPayload());
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
void Client::handleCommand(GameCommand* _cmd) {
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

ServerMessage* Client::getServerMessageFromMsg(Message* _msg) {
  ServerMessage* serverMsg = new ServerMessage();

  bool hasCmd = _msg->getBool(0);
  bool hasState = _msg->getBool(1);
  bool hasExtraPayload = _msg->getBool(2);

  int index = 0;

  serverMsg->setType(_msg->getInt(0));
  if (hasCmd) serverMsg->setCommandList(getCommandListFromMsg(_msg->getMessage(index++)));
  if (hasState) serverMsg->setStateList(getObjectListFromMsg(_msg->getMessage(index++)));
  if (hasExtraPayload) serverMsg->addExtraPayload(_msg->getMessage(index++));

  return ServerMessage;
}

std::list<Command*> Client::getCommandListFromMsg(Message* _msg) {
  std::list<Command*> cmdList;
  int commandCount = _msg->getInt();
  for (int index = 0 ; index < commandCount ; index++) {
    cmdList.push_back(getCommandFromMsg(_msg->getMessage(index)));
  }
  return cmdList;
}

Command* Client::getCommandFromMsg(Message* _msg) {
  Command* comm = new Command();
  comm->setTargetList(getObjectListFromMsg(_msg->getMessage(0)));
  comm->setContent(_msg->getMessage(1));
  comm->setOptimisticallyDeliverable(_msg->getBool(0));
  comm->setConservativelyDeliverable(_msg->getBool(1));
}

std::list<Object*> Client::getObjectListFromMsg(Message* _msg) {
  std::list<Object*> objList;
  int objCount = _msg->getInt();
  for (int index = 0 ; index < objCount ; index++) {
    objList.push_back(getObjectFromMsg(_msg->getMessage(index)));
  }
  return objList;
}

Object* Client::getObjectFromMsg(Message* _msg) {
  Object* obj = objModel->createObject();
  obj->setId(_msg->getInt(0));
  obj->unpackFromNetwork(_msg->getMessage(0));
  return obj;
}
