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

#define GRP_ID_LEN 1000

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
    //NodeInfo* getNodeInfo();
    //void setNodeInfo(NodeInfo* _info);

    int checkNewMessages();
    int checkConnections();
    void handleClientConnect(netwrapper::RemoteFRC* _newClient);
    void handleClientDisconnect(netwrapper::RemoteFRC* _client);
    void handleClientMessage(netwrapper::Message* _msg);
    void handlePeerMessage(netwrapper::Message* _msg);

    void setCallbackInterface(optpaxos::ServerInterface* _cbInterface);
    virtual void handleOptimisticDelivery(Command* _cmd) = 0;

    //debug purposes: the server itself creates a msg
    void sendCommand(Command* cmd, long _clSeq, int _clId);

  private:

    //=======================
    //*** PRIVATE METHODS ***
    //=======================
    int tryProposingPendingCommands();
    void handleLearntValue(OPMessage* _learntMsg);
    void sendCommandToClients(Command* _cmd, CommandType _cmdType);
    void fwdCommandOptimistically(Command* _cmd);
    void fwdCommandToCoordinator(Command* _cmd);
    void handleCommandOneGroup(Command* _cmd);
    void handleCommandMultipleGroups(Command* _cmd);
    long getTime();
    void enqueueOptCmd(Command* _cmd);
    void flushOptCmdQueue();
    //=======================


    //=======================
    //** PRIVATE VARIABLES **
    //=======================
    Group* localGroup;
    std::map<unsigned long, PaxosInstance*> paxosInstances;
    netwrapper::UnreliablePeer* groupPeer;
    netwrapper::FIFOReliableServer* netServer;
    optpaxos::ServerInterface* callbackServer;
    //NodeInfo* nodeInfo;
    long lastPaxosInstance;
    long lastCommandId;
    std::list<netwrapper::RemoteFRC*> clientList; // TODO: create a decent client management
    // TODO: define a way to uniquely identify each server (nodeinfo->id)
    std::list<Command*> optDeliveryQueue;
    long waitWindow;
    //=======================
};

}

#endif /* CORESERVER_H_ */
