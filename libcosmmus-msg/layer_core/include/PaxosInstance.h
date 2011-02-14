/*
 * PaxosInstance.h
 *
 *  Created on: 10/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef PAXOSINSTANCE_H_
#define PAXOSINSTANCE_H_

#include <list>
#include <map>

#include "../../layer_network/include/Message.h"
#include "../../layer_network/include/UnreliablePeer.h"

/*
 *
 */
namespace optpaxos {

class Command;
class Group;
class OPMessage;

class PaxosInstance {
  public:
    PaxosInstance();
    PaxosInstance(long _instanceId);
    void init();
    virtual ~PaxosInstance();
    void flushToDisk();

    long getId();
    void setId(long _id);

    void addAcceptors(Group* _acceptors);
    std::list<Group*> getAcceptors();

    void addLearners(Group* _learners);
    std::list<Group*> getLearners();

    void broadcast(Command* _cmd);
    void broadcastAcceptedValue();
    void broadcastToLearners();

    static void setPeerInterface(netwrapper::UnreliablePeer* _peer);
    static void handleAcceptMessage(OPMessage* _accMsg);
    static void handleAcceptedMessage(OPMessage* _accedMsg);

    static netwrapper::Message* packToNetwork(PaxosInstance* _instance);
    static PaxosInstance* unpackFromNetwork(netwrapper::Message* _msg);

  private:
    long instanceSeq;
    int acceptedMsgCounter;
    //static file paxosLog;
    std::list<Group*> acceptors;
    std::list<Group*> learners;
    OPMessage* acceptedValue;
    bool learnt;
    static netwrapper::UnreliablePeer* peerInterface;
    static std::map<long, PaxosInstance*> instancesIndex;
};

}

#endif /* PAXOSINSTANCE_H_ */