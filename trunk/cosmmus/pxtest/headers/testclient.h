/*
 * testclient.h
 *
 *  Created on: 01/04/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef TESTCLIENT_H_
#define TESTCLIENT_H_

#include <cosmmus-msg.h>

using namespace optpaxos;

/*
 *
 */
class testclient : public optpaxos::Client,
                   public optpaxos::ClientInterface {
  public:
    testclient();
    virtual ~testclient();

    void handleMessage(netwrapper::Message* _msg);

    void sendCommand(Command* cmd, long _clSeq, int _clId);
};

#endif /* TESTCLIENT_H_ */
