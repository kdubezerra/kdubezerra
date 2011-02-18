/*
 * FIFOReliableClient.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>
#include "../include/FIFOReliableClient.h"

using namespace std;
using namespace netwrapper;

FIFOReliableClient::FIFOReliableClient() {
  // TODO Auto-generated constructor stub

}

FIFOReliableClient::~FIFOReliableClient() {
  // TODO Auto-generated destructor stub
}

void FIFOReliableClient::setCallbackInterface(ClientInterface* _callbackClient) {
  clientInterface = _callbackClient;
}

ClientInterface* FIFOReliableClient::getCallbackClient() {
  return clientInterface;
}

int FIFOReliableClient::connect(std::string _address, unsigned _port) {
  IPaddress ip;

  if(SDLNet_ResolveHost(&ip, _address.c_str(), (Uint16) _port)==-1) {
      cerr << "FIFOReliableClient::connect: SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
      return 1;
  }

  clientSocket = SDLNet_TCP_Open(&ip);
  if(!clientSocket) {
      cerr << "FIFOReliableClient::connect: SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
      return 2;
  }

  return 0;
}

int FIFOReliableClient::disconnect() {
  return 0;
}

int FIFOReliableClient::sendMessage(Message* _msg) {
  char* msgBuffer = _msg->getSerializedMessage();
  int msgLength = _msg->getSerializedLength();
  if (SDLNet_TCP_Send(clientSocket, msgBuffer, msgLength) < msgLength) {
    cerr << "FIFOReliableClient::sendMessage: Failed to send message length: " << SDLNet_GetError() << endl;
    return 1;
  }
  cout << "FIFOReliableClient::sendMessage: ";
  for (int i = 0 ; i < msgLength ; i++) cout << msgBuffer[i];
  cout << endl;
  delete [] msgBuffer;
  return 0;
}
