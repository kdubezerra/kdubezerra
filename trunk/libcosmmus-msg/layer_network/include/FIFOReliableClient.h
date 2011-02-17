/*
 * FIFOReliableClient.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef FIFORELIABLECLIENT_H_
#define FIFORELIABLECLIENT_H_

#include <string>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#include "../include/GenericNode.h"

/*
 *
 */
namespace netwrapper {

class ClientInterface;
class Message;

class FIFOReliableClient : public GenericNode {
  public:
    FIFOReliableClient();
    virtual ~FIFOReliableClient();

    void setCallbackInterface(netwrapper::ClientInterface* _callbackClient);
    netwrapper::ClientInterface* getCallbackClient();
    int connect(std::string _address, unsigned _port);
    int disconnect();
    int sendMessage(Message* _msg);

  private:
    netwrapper::ClientInterface* clientInterface;
    TCPsocket clientSocket;

};

}

#endif /* FIFORELIABLECLIENT_H_ */
