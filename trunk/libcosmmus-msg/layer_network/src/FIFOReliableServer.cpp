/*
 * FIFOReliableServer.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>

#include "../include/FIFOReliableServer.h"
#include "../include/RemoteFRC.h"

using namespace std;
using namespace netwrapper;

FIFOReliableServer::FIFOReliableServer() {
  // TODO Auto-generated constructor stub

}

FIFOReliableServer::~FIFOReliableServer() {
  // TODO Auto-generated destructor stub
}

int FIFOReliableServer::init (unsigned _port) {
  IPaddress ip;

  if (SDLNet_Init() < 0) {
    cerr << "SDLNet_Init: " << SDLNet_GetError() << endl;
    return(1);
  }

  if(SDLNet_ResolveHost(&ip, NULL, (Uint16) _port)==-1) {
    cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
    return(2);
  }

  serverSocket = SDLNet_TCP_Open(&ip);
  if(!serverSocket) {
    cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
    return(3);
  }

  return 0;
}

void FIFOReliableServer::send (Message* _msg, RemoteFRC* _client) {

}
