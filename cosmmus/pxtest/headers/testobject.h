/*
 * testobject.h
 *
 *  Created on: 22/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef TESTOBJECT_H_
#define TESTOBJECT_H_

#include <list>
#include <cosmmus-msg.h>

/*
 *
 */
class testobject : public optpaxos::Object {
  public:
    testobject();
    testobject(testobject* _other);
    testobject(int _id);
    virtual ~testobject();

    void handleNewOptimisticState(Object* _state);
    void handleNewConservativeState(Object* _state);
    void handleOptimisticDelivery(optpaxos::Command* _cmd);
    void handleConservativeDelivery(optpaxos::Command* _cmd);
    int optState;
    int conState;
    std::list<long> optComList, consComList;
    void printCmdLists();

};

#endif /* TESTOBJECT_H_ */
