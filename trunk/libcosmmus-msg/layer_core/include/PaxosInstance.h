/*
 * PaxosInstance.h
 *
 *  Created on: 10/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef PAXOSINSTANCE_H_
#define PAXOSINSTANCE_H_

/*
 *
 */
namespace optpaxos {

class PaxosInstance {
  public:
    PaxosInstance();
    PaxosInstance(int _instanceId);
    virtual ~PaxosInstance();
    void flushToDisk();
    void addAcceptors(Group* _acceptors);
    void addLearners(Group* _learners);
    void broadCast(Command* _cmd);

    static void setPeerInterface(netwrapper::UnreliablePeer* _peer);
    static void handleAcceptMessage(OPMessage* _accMsg);

  private:
    int instanceSeq;
    //static file paxosLog;
    std::list<Group*> acceptors;
    std::list<Group*> learners;
    OPMessage* acceptedValue;
    bool learnt;
    static netwrapper::UnreliablePeer* peerInterface;
    static std::list<PaxosInstance*> instanceList;
};

}

#endif /* PAXOSINSTANCE_H_ */
