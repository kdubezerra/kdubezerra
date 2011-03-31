/*
 * RemoteFRC.cpp
 *
 *  Created on: 17/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/RemoteFRC.h"

using namespace netwrapper;

RemoteFRC::RemoteFRC() {
  connected = false;
}

RemoteFRC::RemoteFRC(TCPsocket _clientSocket) {
  clientSocket = _clientSocket;
  connected = true;
}

RemoteFRC::~RemoteFRC() {
  // TODO Auto-generated destructor stub
}

bool RemoteFRC::equals(RemoteFRC* _other) {
  if (this->clientSocket != _other->clientSocket)
    return false;
  return true;
}

TCPsocket RemoteFRC::getSocket() const {
    return clientSocket;
}

void RemoteFRC::setSocket(TCPsocket _clientSocket) {
    clientSocket = _clientSocket;
}

bool RemoteFRC::isConnected() {
  return connected;
}

void RemoteFRC::setConnected(bool _isConnected) {
  connected = _isConnected;
}
