/*
 * Client.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>

// core level classes
#include "../include/Client.h"
#include "../include/Object.h"
#include "../include/OPMessage.h"
#include "../include/ObjectInfo.h"

// network level classes
#include "../../layer_network/include/FIFOReliableClient.h"

using namespace std;
using namespace optpaxos;
using namespace netwrapper;

Client::Client() {
  netClient = new FIFOReliableClient();
  netClient->setCallbackInterface(this);
}

Client::~Client() {
  // TODO Auto-generated destructor stub
}

int Client::connect(std::string _address, unsigned _port) {
  return netClient->connect(_address, _port);
}

void Client::disconnect() {
  return netClient->disconnect();
}

void Client::submitCommand(Command* _cmd) {
  OPMessage* cmdOpMsg = new OPMessage();
  cmdOpMsg->setType(CLIENT_CMD);
  cmdOpMsg->addCommand(new Command(_cmd));
  Message* packedCmdOpMsg = OPMessage::packToNetwork(cmdOpMsg);
  netClient->sendMessage(packedCmdOpMsg);
  delete packedCmdOpMsg;
  delete cmdOpMsg;
}

void Client::submitRequest(netwrapper::Message* _msg) {
  OPMessage* opMsg = new OPMessage();
  opMsg->setType(APP_MSG);
  opMsg->addMessage(_msg);
  Message* msgOpMsg = OPMessage::packToNetwork(opMsg);
  netClient->sendMessage(msgOpMsg);
  delete msgOpMsg;
  delete opMsg;
}

void Client::handleServerMessage(netwrapper::Message* _msg) {
  OPMessage* serverMsg = OPMessage::unpackFromNetwork(_msg);

  switch (serverMsg->getType()) {
    case CMD_ONE_GROUP_OPTIMISTIC : {
      cout << "Client::handleServerMessage: got an optimistic delivery command" << endl;
      Command* optCmd = serverMsg->getCommandList().front();
      std::list<ObjectInfo*> targetList = optCmd->getTargetList();
      for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++) {
        Object* obj = Object::getObjectById((*it)->getId());
        obj->handleOptimisticDelivery(optCmd);
      }
    }
    break;

    case CMD_ONE_GROUP_CONSERVATIVE : {
      cout << "Client::handleServerMessage: got a conservative delivery command" << endl;
      Command* newCmd = serverMsg->getCommandList().front();
      std::list<ObjectInfo*> targetList = newCmd->getTargetList();
      for (std::list<ObjectInfo*>::iterator it = targetList.begin() ; it != targetList.end() ; it++) {
        Object* obj = Object::getObjectById((*it)->getId());
        obj->handleConservativeDelivery(newCmd);
        obj->getInfo()->setLastStamp(newCmd->getLogicalStamp());
      }
    }
    break;

    default: {

    }
    break;
  }

  delete serverMsg;
}

void Client::checkAll() {
  checkNewMessages();
}

void Client::checkNewMessages() {
  netClient->checkNewMessages();
}

void Client::handleStateUpdate(Object* _state) {
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
