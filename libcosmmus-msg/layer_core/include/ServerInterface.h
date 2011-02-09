/*
 * ServerInterface.h
 *
 *  Created on: 09/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef SERVERINTERFACE_H_
#define SERVERINTERFACE_H_

#include "../../layer_network/include/Message.h"

/*
 *
 */
namespace optpaxos {

class Command;

class ServerInterface {
  public:
    ServerInterface();
    virtual ~ServerInterface();

    virtual void handleClientMessage(netwrapper::Message* _msg) = 0;
    virtual void defineServers(Command* _cmd) = 0;
};

}

#endif /* SERVERINTERFACE_H_ */
