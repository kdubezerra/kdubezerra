/*
 * CoreClient.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

// api level classes
#include "../../include/GameClient.h"
#include "../../include/Command.h"
#include "../../include/Object.h"

// core level classes
#include "../include/CoreClient.h"
#include "../include/ServerMessage.h"

using namespace std;

CoreClient::CoreClient() {
  // TODO Auto-generated constructor stub

}

CoreClient::~CoreClient() {
  // TODO Auto-generated destructor stub
}

int CoreClient::connect(string _address) {
  return 0;
}

int CoreClient::disconnect() {
  return 0;
}

void CoreClient::submitCommand(Command* _cmd) {

}

void CoreClient::setCallbackClientInterface(GameClient* _cbclient) {
  cbclient = _cbclient;
}

GameClient* CoreClient::getCallbackClientInterface() {
  return cbclient;
}

void CoreClient::handleServerMessage(ServerMessage* _serverMsg) {
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
    getCallbackClientInterface()->handleServerAppMsg(_serverMsg->getExtraPayload());
      cout << "Error while processing the ServerMessage " << this << endl;
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
void CoreClient::handleCommand(Command* _cmd) {
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
      // TODO: at this point, the delivered command must be checked against the first one in the optimistic queue.
      //       Should be found a difference, action must be taken to correct this discrepancy.
      obj->handleConservativeDelivery(_cmd->getContent());
  }
}

void handleStateUpdate(Object* _state) {
/* TODO: create a coherent model of state and its updates: when the server sends an update, what
 *       is it updating? There at least two and, possibly, one more state delivery queue. How is
 *       this going to work? When will one state overwrite the other? Every object should have a
 *       command queue and the library is responsible for controlling this? Maybe this should be
 *       delegated do the application? Although this is not critical for the development of this
 *       library, it would be interest to have this problem worked out.
 *
 * UPDATE: for now, the "visible" state will be the optimistic one, which will be overwritten and
 *     recalculated once a discrepancy between the conservative delivery order and the previously
 *     considered conservative one.
 */

}
