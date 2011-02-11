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
    virtual ~PaxosInstance();
    void flushToDisk();
    void addAcceptors(Group* _acceptors);
    void addLearners(Group* _learners);

  private:
    instanceId;
    //static file paxosLog;
    std::list<Group*> acceptorsList;
    std::list<Group*> learnersList;
    OPMessage* acceptedValue;
    bool learnt;
};

}

#endif /* PAXOSINSTANCE_H_ */
