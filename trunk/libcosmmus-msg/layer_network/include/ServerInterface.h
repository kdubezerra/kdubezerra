/*
 * ServerInterface.h
 *
 *  Created on: 10/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef SERVERINTERFACE_H_
#define SERVERINTERFACE_H_

#include "../../layer_network/include/Address.h"

namespace netwrapper {

class Message;

class ServerInterface {
  public:
    ServerInterface();
    virtual ~ServerInterface();

    virtual void handleClientConnect(Address* _newClient) = 0;
    virtual void handleClientDisconnect(Address* _client) = 0;
    virtual void handleClientMessage(Message* _msg) = 0;
};

}

#endif /* SERVERINTERFACE_H_ */
