/*
 * FIFOReliableServer.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef FIFORELIABLESERVER_H_
#define FIFORELIABLESERVER_H_

#include <list>
#include <SDL/SDL_net.h>

#include "GenericNode.h"
#include "Message.h"

namespace netwrapper {

#define MAX_CLIENTS 10000

class Message;
class RemoteFRC;
class ServerInterface;

class FIFOReliableServer {
  public:
    FIFOReliableServer();
    virtual ~FIFOReliableServer();

    int init(unsigned _port);
    void setCallbackInterface(ServerInterface* _cbServer);

    int checkConnections();
    int checkNewMessages();
    int send (Message* _msg, RemoteFRC* _client);
    void disconnect(RemoteFRC* _client);
    std::list<RemoteFRC*> getClientList() const;
    void setClientList(std::list<RemoteFRC*> clientList);

  private:
    ServerInterface* callbackServer;
    std::list<RemoteFRC*> clientList;
    TCPsocket serverSocket;
    SDLNet_SocketSet socketSet;
};

}


#endif /* FIFORELIABLESERVER_H_ */
