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

  private:
    instanceId;
    std::list<GroupInfo*> acceptorsList;
    std::list<GroupInfo*> learnersList;
    OPMessage* acceptedValue;
    bool learnt;
};

}

#endif /* PAXOSINSTANCE_H_ */
