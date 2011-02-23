/*
 * FIFOReliableClient.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef FIFORELIABLECLIENT_H_
#define FIFORELIABLECLIENT_H_

#include <string>
#include <SDL/SDL_net.h>

#include "GenericNode.h"
#include "Message.h"

/*
 *
 */
namespace netwrapper {

class ClientInterface;
class Message;

class FIFOReliableClient {
  public:
    FIFOReliableClient();
    virtual ~FIFOReliableClient();

    void setCallbackInterface(netwrapper::ClientInterface* _callbackClient);
    netwrapper::ClientInterface* getCallbackInterface();
    int connect(std::string _address, unsigned _port);
    void disconnect();
    int sendMessage(Message* _msg);
    int checkNewMessages();

  private:
    netwrapper::ClientInterface* callbackClient;
    TCPsocket clientSocket;
    SDLNet_SocketSet socketSet;

};

}

#endif /* FIFORELIABLECLIENT_H_ */
