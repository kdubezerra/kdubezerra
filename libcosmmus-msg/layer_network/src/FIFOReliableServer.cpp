/*
 * FIFOReliableServer.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>

#include "../include/Address.h"
#include "../include/FIFOReliableServer.h"
#include "../include/RemoteFRC.h"
#include "../include/ServerInterface.h"

using namespace std;
using namespace netwrapper;

FIFOReliableServer::FIFOReliableServer() {
  callbackServer = NULL;
  address = NULL;
}

FIFOReliableServer::~FIFOReliableServer() {
  // TODO Auto-generated destructor stub
}

int FIFOReliableServer::init (unsigned _port) {
  IPaddress ip;

  if (SDLNet_Init() < 0) {
    cerr << "FIFOReliableServer::init: SDLNet_Init: " << SDLNet_GetError() << endl;
    return(1);
  }

  if(SDLNet_ResolveHost(&ip, NULL, (Uint16) _port)==-1) {
    cerr << "FIFOReliableServer::init: SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
    return(2);
  }

  serverSocket = SDLNet_TCP_Open(&ip);
  if(serverSocket == NULL) {
    cerr << "FIFOReliableServer::init: SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
    return(3);
  }

  socketSet = SDLNet_AllocSocketSet(MAX_CLIENTS);
  if (socketSet == NULL) {
    cerr << "FIFOReliableServer::init: Failed to allocate the socket set: " << SDLNet_GetError() << endl;
    return(4);
  }

  SDLNet_TCP_AddSocket(socketSet, serverSocket);

  cout << "FIFOReliableServer::init: Server initialization completed successfully." << endl;
  return 0;
}

void FIFOReliableServer::setCallbackServer(ServerInterface* _cbServer) {
  callbackServer = _cbServer;
}

void FIFOReliableServer::checkConnections() {
  TCPsocket clientSocket;
  IPaddress* clientIP;

  if ((clientSocket = SDLNet_TCP_Accept(serverSocket))) {
    if ((clientIP = SDLNet_TCP_GetPeerAddress(clientSocket))) {
      SDLNet_TCP_AddSocket(socketSet, clientSocket);
      RemoteFRC* newClient = new RemoteFRC();
      newClient->setSocket(clientSocket);
      clientList.push_back(newClient);
      if (callbackServer) callbackServer->handleClientConnect(newClient);
      cout << "FIFOReliableServer::checkConnections: Host connected: " << SDLNet_Read32(&clientIP->host) << ":" << SDLNet_Read16(&clientIP->port) << endl;
    }
    else
      cerr << "FIFOReliableServer::checkConnections: SDLNet_TCP_GetPeerAddress: " << SDLNet_GetError() << endl;
  }

  //check disconnections
}

void FIFOReliableServer::checkNewMessages() {
  int socketActivity = SDLNet_CheckSockets(socketSet, 0);
  cout << "FIFOReliableServer::checkNewMessages: socketActivity = " << socketActivity << endl;

  for (std::list<RemoteFRC*>::iterator it = clientList.begin() ; it != clientList.end() ; it++) {
    int clientSocketActivity = SDLNet_SocketReady((*it)->getSocket());
    cout << "FIFOReliableServer::checkNewMessages: clientSocketActivity for client " << *it << " = " << socketActivity << endl;
    if (clientSocketActivity != 0) {
      char lengthBuffer[4];
      int receivedByteCount = SDLNet_TCP_Recv((*it)->getSocket(), lengthBuffer, 4);
      if (receivedByteCount <= 0) {
        cout << "FIFOReliableServer::checkNewMessages: client apparently disconnected" << endl;
        disconnect(*it);
        continue;
      }
      int messageLength = SDLNet_Read32(lengthBuffer);
      cout << "FIFOReliableServer::checkNewMessages: received message length = " << messageLength << endl;
      char* messageBuffer = new char[messageLength];
      memcpy(messageBuffer, lengthBuffer, 4);
      receivedByteCount = SDLNet_TCP_Recv((*it)->getSocket(), messageBuffer + 4, messageLength - 4);
      if (receivedByteCount <= 0) {
        cout << "FIFOReliableServer::checkNewMessages: client apparently disconnected" << endl;
        disconnect(*it);
        continue;
      }
      cout << "FIFOReliableServer::checkNewMessages: new received message: ";
      for (int i = 0 ; i < messageLength ; i++) cout << messageBuffer[i];
      cout << endl;
      Message* rcvdMessage = new Message();
      rcvdMessage->buildFromBuffer(messageBuffer);
      delete [] messageBuffer;
      if (callbackServer) callbackServer->handleClientMessage(rcvdMessage);
      delete rcvdMessage;
    }
  }
}

void FIFOReliableServer::send (Message* _msg, RemoteFRC* _client) {

}

void FIFOReliableServer::disconnect(RemoteFRC* _client) {
  clientList.remove(_client);
  SDLNet_TCP_DelSocket(socketSet, _client->getSocket());
  SDLNet_TCP_Close(_client->getSocket());
  callbackServer->handleClientDisconnect(_client);
  delete _client;
}
