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

#include "PaxosLearnerInterface.h"

#include "../../layer_network/include/FIFOReliableServer.h"
#include "../../layer_network/include/Message.h"
#include "../../layer_network/include/PeerInterface.h"
#include "../../layer_network/include/ServerInterface.h"
#include "../../layer_network/include/UnreliablePeer.h"

namespace optpaxos {

#define SRV_ID_LEN 10000

class Command;
class Group;
class NodeInfo;
class OPMessage;
class PaxosInstance;
class PaxosLearnerInterface;
class ServerInterface;

class Server : public optpaxos::PaxosLearnerInterface,
               public netwrapper::ServerInterface,
               public netwrapper::PeerInterface {
  public:
    Server();
    virtual ~Server();

    int init(unsigned _reliablePort, unsigned _unreliablePort);
    int joinGroup(Group *_group);
    void leaveGroup();
    void handleClientConnect(netwrapper::Address* _newClient);
    void handleClientDisconnect(netwrapper::Address* _client);

    NodeInfo* getNodeInfo();
    void setNodeInfo(NodeInfo* _info);

    void handleClientMessage(netwrapper::Message* _msg);
    void handlePeerMessage(netwrapper::Message* _msg);
    void handleLearntValue(OPMessage* _learntMsg);

    void setCallbackInterface(optpaxos::ServerInterface* _cbInterface);
    virtual void handleOptimisticDelivery();

  private:
    void sendCommandToClients(Command* _cmd);
    void fwdOptimisticallyToGroups(Command* _cmd);
    void fwdCommandToCoordinator(Command* _cmd);
    void handleCommandOneGroup(Command* _cmd);
    void handleCommandMultipleGroups(Command* _cmd);
    Group* localGroup;
    std::map<unsigned long, PaxosInstance*> paxosInstances;
    netwrapper::UnreliablePeer* groupPeer;
    netwrapper::FIFOReliableServer* netServer;
    optpaxos::ServerInterface* callbackServer;
    NodeInfo* nodeInfo;
    long lastPaxosInstance;
    std::list<netwrapper::Address*> clientList; // TODO: create a decent client management
    // TODO: define a way to uniquely identify each server (ip:port?)
};

}

#endif /* CORESERVER_H_ */
