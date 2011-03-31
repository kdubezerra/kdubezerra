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

#include "PaxosLearnerInterface.h"

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
    static void flushToDisk(PaxosInstance* _pi);
    static void flushAll();
    static PaxosInstance* findInstance(long _seq);

    long getId();
    void setId(long _id);

    void addAcceptors(Group* _acceptors);
    void setAcceptorsGroups(std::list<Group*> _accList);
    std::list<Group*> getAcceptors();

    void addLearners(Group* _learners);
    void setLearnersGroups(std::list<Group*> _lList);
    std::list<Group*> getLearners();

    void broadcast(OPMessage* _cmd);
    static void handleAcceptMessage(OPMessage* _accMsg);
    void broadcastAcceptedValue();
    static void handleAcceptedMessage(OPMessage* _accedMsg);

    static void setLearner(PaxosLearnerInterface* _learner);
    static void setPeerInterface(netwrapper::UnreliablePeer* _peer);
    static netwrapper::Message* packToNetwork(PaxosInstance* _instance);
    static PaxosInstance* unpackFromNetwork(netwrapper::Message* _msg);

  private:
    long instanceSeq;
    int acceptedMsgCounter;
    //static file paxosLog;
    std::list<Group*> acceptorsGroupsList;
    std::list<Group*> learnersGroupsList;
    OPMessage* acceptedValue;
    bool learnt;
    static PaxosLearnerInterface* callbackLearner;
    static netwrapper::UnreliablePeer* peerInterface;
    static std::map<long, PaxosInstance*> instancesIndex;
    static std::list<PaxosInstance*> flushableList;
};

}

#endif /* PAXOSINSTANCE_H_ */
