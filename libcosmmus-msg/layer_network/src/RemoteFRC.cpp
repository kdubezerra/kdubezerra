/*
 * RemoteFRC.cpp
 *
 *  Created on: 17/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/RemoteFRC.h"

using namespace netwrapper;

RemoteFRC::RemoteFRC() {
  // TODO Auto-generated constructor stub
}

RemoteFRC::~RemoteFRC() {
  // TODO Auto-generated destructor stub
}

TCPsocket RemoteFRC::getSocket() const {
    return clientSocket;
}

void RemoteFRC::setSocket(TCPsocket _clientSocket) {
    clientSocket = _clientSocket;
}
