/*
 * FIFOReliableClient.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>
#include "../include/ClientInterface.h"
#include "../include/FIFOReliableClient.h"

using namespace std;
using namespace netwrapper;

FIFOReliableClient::FIFOReliableClient() {
  socketSet = SDLNet_AllocSocketSet(1);
  if (socketSet == NULL) {
    cout << "FIFOReliableClient::FIFOReliableClient: ERROR: socketSet NOT allocated!" << endl;
  }
  callbackClient = NULL;
}

FIFOReliableClient::~FIFOReliableClient() {
  // TODO Auto-generated destructor stub
}

void FIFOReliableClient::setCallbackInterface(ClientInterface* _callbackClient) {
  callbackClient = _callbackClient;
}

ClientInterface* FIFOReliableClient::getCallbackInterface() {
  return callbackClient;
}

int FIFOReliableClient::connect(std::string _address, unsigned _port) {
  IPaddress ip;

  cout << 1 << endl;
  if(SDLNet_ResolveHost(&ip, _address.c_str(), (Uint16) _port)==-1) {
    cerr << "FIFOReliableClient::connect: SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
    return 1;
  }

  cout << 2 << endl;
  clientSocket = SDLNet_TCP_Open(&ip);
  if(clientSocket == NULL) {
    cerr << "FIFOReliableClient::connect: SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
    return 2;
  }

  cout << 3 << endl;
  if (socketSet == NULL) {
    cerr << "FIFOReliableClient::connect: SDLNet_AllocSocketSet: " << SDLNet_GetError() << endl;
    return 3; // Quit!
  }

  cout << 4 << endl;
  SDLNet_TCP_AddSocket(socketSet, clientSocket);
  cout << "FIFOReliableClient::connect: connected to server at " << _address << ":" << _port << endl;

  cout << 5 << endl;
  return 0;
}

void FIFOReliableClient::disconnect() {
  SDLNet_TCP_Close(clientSocket);
  SDLNet_TCP_DelSocket(socketSet, clientSocket);
  cout << "FIFOReliableClient::disconnect: client disconnected from server" << endl;
}

int FIFOReliableClient::sendMessage(Message* _msg) {
  char* msgBuffer = _msg->getSerializedMessage();
  int msgLength = _msg->getSerializedLength();
  if (SDLNet_TCP_Send(clientSocket, msgBuffer, msgLength) < msgLength) {
    cerr << "FIFOReliableClient::sendMessage: Failed to send message length: " << SDLNet_GetError() << endl;
    return 1;
  }
  delete [] msgBuffer;
  return 0;
}

int FIFOReliableClient::checkNewMessages() {
  int rcvdMsgCount = 0;
  int socketActivity = SDLNet_CheckSockets(socketSet, 0);
  cout << "FIFOReliableClient::checkNewMessages: socketActivity = " << socketActivity << endl;

  int clientSocketActivity = SDLNet_SocketReady(clientSocket);
  cout << "FIFOReliableClient::checkNewMessages: clientSocketActivity for client " << this << " = " << socketActivity << endl;
  if (clientSocketActivity != 0) {
    char lengthBuffer[4];
    int receivedByteCount = SDLNet_TCP_Recv(clientSocket, lengthBuffer, 4);
    if (receivedByteCount <= 0) {
      cout << "FIFOReliableClient::checkNewMessages: disconnected from server" << endl;
      disconnect();
      return 1;
    }
    int messageLength = SDLNet_Read32(lengthBuffer);
    cout << "FIFOReliableClient::checkNewMessages: received message length = " << messageLength << endl;
    char* messageBuffer = new char[messageLength];
    memcpy(messageBuffer, lengthBuffer, 4);
    receivedByteCount = SDLNet_TCP_Recv(clientSocket, messageBuffer + 4, messageLength - 4);
    if (receivedByteCount <= 0) {
      cout << "FIFOReliableClient::checkNewMessages: disconnected from server" << endl;
      disconnect();
      return 2;
    }
    rcvdMsgCount++;
    cout << "FIFOReliableClient::checkNewMessages: new received message: ";
    for (int i = 0 ; i < messageLength ; i++) cout << messageBuffer[i];
    cout << endl;
    Message* rcvdMessage = new Message();
    rcvdMessage->buildFromBuffer(messageBuffer);
    delete [] messageBuffer;
    if (callbackClient) callbackClient->handleServerMessage(rcvdMessage);
    delete rcvdMessage;
  }
  return rcvdMsgCount;
}
