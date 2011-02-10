/*
 * Server.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <list>
#include <string>
#include "../../layer_network/include/FIFOReliableServer.h"
#include "../../layer_network/include/Message.h"
#include "../../layer_network/include/UnreliablePeer.h"

namespace optpaxos {

class Command;
class Group;

/*
 *
 */
class Server : public netwrapper::ServerInterface,
               public netwrapper::PeerInterface,
               private OPTPaxosControl {
  public:
    Server();
    virtual ~Server();

    int init(unsigned _reliablePort, unsigned _unreliablePort);
    int joinGroup(Group *_group);
    void leaveGroup();
    void handlePeerMessage(netwrapper::Message* _msg);
    void handleClientMessage(netwrapper::Message _msg);

  private:
    void handleClientCommand(Command* _cmd);
    void handleCommandInsideGroup(Command* _cmd);
    void handleCommandToOtherGroups(Command* _cmd);
    netwrapper::UnreliablePeer* groupPeer;
    netwrapper::FIFOReliableServer* netServer;
    ServerInterface* callbackServer;
    // TODO: define a way to uniquely identify each server (ip:port?)
};

}

#endif /* CORESERVER_H_ */
