/*
 * UnreliablePeer.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>
#include "../include/UnreliablePeer.h"

using namespace std;
using namespace netwrapper;

UnreliablePeer::UnreliablePeer() {
  callbackPeer = NULL;
}

UnreliablePeer::~UnreliablePeer() {
  // TODO Auto-generated destructor stub
}

int UnreliablePeer::init(unsigned _port) {

  if (SDLNet_Init() < 0) {
    cerr << "UnreliablePeer::init: SDLNet_Init: " << SDLNet_GetError() << endl;
    return 1;
  }

  peerSocket = SDLNet_UDP_Open((Uint16) _port);
  if (peerSocket == NULL) {
    cerr << "UnreliablePeer::init: SDLNet_UDP_Open: " << SDLNet_GetError() << endl;
    return 2;
  }

  return 0;
}

int UnreliablePeer::sendMessage(Message* _msg, Address* _address) {
  UDPpacket* packet;
  int messageLength = _msg->getSerializedLength();
  char* messageBuffer = _msg->getSerializedMessage();

  packet = SDLNet_AllocPacket(messageLength);
  if (packet == NULL) {
    cerr << "UnreliablePeer::sendMessage: SDLNet_AllocPacket: " << SDLNet_GetError() << endl;
    return 1;
  }

  memcpy(packet->data, messageBuffer, messageLength);
  packet->address = _address->getAddress();
  packet->len = messageLength;
  SDLNet_UDP_Send(peerSocket, -1, packet);

  SDLNet_FreePacket(packet);
  delete [] messageBuffer;

  return 0;
}

int UnreliablePeer::checkNewMessages() {
  UDPpacket* rcvPacket = SDLNet_AllocPacket(8192);
  int recvCount = 0;

  if (rcvPacket == NULL) {
    cerr << "UnreliablePeer::checkNewMessages: SDLNet_AllocPacket: " << SDLNet_GetError() << endl;
    return 1;
  }

  if (SDLNet_UDP_Recv(peerSocket, rcvPacket)) {
    //cout << "UnreliablePeer::checkNewMessages: received UDP PACKET of length " << rcvPacket->len << " bytes" << endl;
    recvCount++;
    Message* rcvMsg = new Message((char *)(rcvPacket->data));
    //cout << "UnreliablePeer::checkNewMessages: received netwrapper::Message of length " << rcvMsg->getSerializedLength() << " bytes" << endl;
    if (callbackPeer) callbackPeer->handlePeerMessage(rcvMsg);
    delete rcvMsg;
  }
  SDLNet_FreePacket(rcvPacket);

  return recvCount;
}

void UnreliablePeer::setCallbackInterface(PeerInterface* _callbackPeer) {
  callbackPeer = _callbackPeer;
}

PeerInterface* UnreliablePeer::getCallbackInterface() {
  return callbackPeer;
}
