/*
 * CoreClient.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

// api level classes
#include "../../include/GameClient.h"
#include "../../include/GameClient.h"

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

}

int CoreClient::disconnect() {

}

int CoreClient::submitCommand(Command* _cmd) {

}

int CoreClient::handleServerMessage(ServerMessage* _serverMsg) {
  if (_serverMsg->hasState()) {
    list<Object*> states = _serverMsg->getStateList();
    for (list<Object*>::iterator it = states.begin() ; it != states.end() ; it++) {
      if (handleStateUpdate(*it))
        cout << "Error while updating the State " << *it << endl;
    }
  }

  if (_serverMsg->hasCommand()) {
    list<Command*> coms = _serverMsg->getCommandList();
    for (list<Command*>::iterator it = coms.begin() ; it != coms.end() ; it++) {
      if (handleCommand(*it))
        cout << "Error while processing the Command " << *it << endl;
    }
  }

  if (_serverMsg->hasExtraPayload()) {
    if (this->getCallbackClientInterface()->handleServerAppMsg(_serverMsg->getExtraPayload()))
      cout << "Error while processing the ServerMessage " << this << endl;
  }
}

int CoreClient::setCallbackClientInterface(GameClient* _cbclient) {

}
