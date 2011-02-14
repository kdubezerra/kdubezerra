/*
 * PaxosInstance.cpp
 *
 *  Created on: 10/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/PaxosInstance.h"

using namespace optpaxos;
using namespace netwrapper;

UnreliablePeer* peerInterface = NULL;

PaxosInstance::PaxosInstance() {
  init();
}

PaxosInstance::PaxosInstance(int _instanceId) {
  init();
  instanceSeq = _instanceId;
}

PaxosInstance::init() {
  acceptedValue = NULL;
  learnt = false;
}

PaxosInstance::~PaxosInstance() {
  // TODO Auto-generated destructor stub
}

void PaxosInstance::broadCast(Command* _cmd) {
  OPMessage* acceptMessage = new OPMessage();
  acceptMessage->setType(ACCEPT_MSG);
  acceptMessage->addCommand(new Command(_cmd));
  acceptMessage->setExtraPayload(PaxosInstance::packToNetwork(this));
  Message* packedAccMsg = OPMessage::packToNetwork(acceptMessage);
  for (std::list<Group*>::iterator itgroup = acceptorsList.begin() ; itgroup != acceptorsList.end() ; itgroup++) {
    std::list<NodeInfo*> group = (*itgroup)->getServerList();
    for (std::list<NodeInfo*>::iterator itnode = group.begin() ; itnode != group.end() ; itnode++) {
      peerInterface->sendMessage(packedAccMsg, (*itnode)->getAdress());
    }
  }
  delete packedAccMsg;
  delete acceptMessage;
}

void PaxosInstance::setPeerInterface(netwrapper::UnreliablePeer* _peer) {
  peerInterface = _peer;
}

void PaxosInstance::handleAcceptMessage(OPMessage* _accMsg) {

}
