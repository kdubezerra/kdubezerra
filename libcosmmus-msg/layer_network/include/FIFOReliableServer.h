/*
 * FIFOReliableServer.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef FIFORELIABLESERVER_H_
#define FIFORELIABLESERVER_H_

#include <list>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#include "GenericNode.h"

namespace netwrapper {

class Message;
class RemoteFRC;

class FIFOReliableServer : public GenericNode {
  public:
    FIFOReliableServer();
    virtual ~FIFOReliableServer();

    int init (unsigned _port);

    void send (Message* _msg, RemoteFRC* _client);

  private:
    std::list<RemoteFRC*> clientList;
    TCPsocket serverSocket;
};

}

#endif /* FIFORELIABLESERVER_H_ */
