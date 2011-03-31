/*
 * testserver.h
 *
 *  Created on: 21/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef TESTSERVER_H_
#define TESTSERVER_H_

/*
 *
 */
#include <cosmmus-msg.h>

class testserver: public optpaxos::Server {
  public:
    testserver();
    virtual ~testserver();
    void handleOptimisticDelivery(optpaxos::Command* _cmd);
};

#endif /* TESTSERVER_H_ */
