/*
 * ServerInterface.h
 *
 *  Created on: 10/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef SERVERINTERFACE_H_
#define SERVERINTERFACE_H_

namespace netwrapper {

class Message;
class RemoteFRC;

class ServerInterface {
  public:
    ServerInterface();
    virtual ~ServerInterface();

    virtual void handleClientConnect(RemoteFRC* _newClient) = 0;
    virtual void handleClientDisconnect(RemoteFRC* _client) = 0;
    virtual void handleClientMessage(Message* _msg) = 0;
};

}

#endif /* SERVERINTERFACE_H_ */
