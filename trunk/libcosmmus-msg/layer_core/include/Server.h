/*
 * Server.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <list>
#include <map>
#include <string>

#include "../../layer_network/include/FIFOReliableServer.h"
#include "../../layer_network/include/Message.h"
#include "../../layer_network/include/PeerInterface.h"
#include "../../layer_network/include/ServerInterface.h"
#include "../../layer_network/include/UnreliablePeer.h"

namespace optpaxos {

#define SVID_LEN 10000

class Command;
class Group;
class NodeInfo;
class OPMessage;
class PaxosInstance;
class ServerInterface;

/*
 *
 */
class Server : public netwrapper::ServerInterface,
               public netwrapper::PeerInterface {
  public:
    Server();
    virtual ~Server();

    int init(unsigned _reliablePort, unsigned _unreliablePort);
    int joinGroup(Group *_group);
    void leaveGroup();

    NodeInfo* getNodeInfo();
    void setNodeInfo(NodeInfo* _info);

    void handleClientMessage(netwrapper::Message* _msg);
    void handlePeerMessage(netwrapper::Message* _msg);
    void handleLearntMessage(OPMessage* _learntMsg);

    void setCallbackInterface(optpaxos::ServerInterface* _cbInterface);
    virtual void handleOptimisticDelivery();

  private:
    void fwdOptimisticallyToGroups(Command* _cmd);
    void fwdCommandToCoordinator(Command* _cmd);
    void handleCommandOneGroup(Command* _cmd);
    void handleCommandMultipleGroups(Command* _cmd);
    Group* localGroup;
    std::map<unsigned long, PaxosInstance*> paxosInstances;
    netwrapper::UnreliablePeer* groupPeer;
    netwrapper::FIFOReliableServer* netServer;
    ServerInterface* callbackServer;
    NodeInfo* nodeInfo;
    long lastPaxosInstance;
    // TODO: define a way to uniquely identify each server (ip:port?)
};

}

#endif /* CORESERVER_H_ */
