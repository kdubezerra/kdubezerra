/*
 * PaxosLearner.h
 *
 *  Created on: 15/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef PAXOSLEARNER_H_
#define PAXOSLEARNER_H_

/*
 *
 */
namespace optpaxos {

class OPMessage;

class PaxosLearnerInterface {
  public:
    PaxosLearnerInterface();
    virtual ~PaxosLearnerInterface();
    virtual void handleLearntValue(OPMessage* _learntValue) = 0;
};

}

#endif /* PAXOSLEARNER_H_ */
