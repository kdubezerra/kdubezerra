/*
 * ClientInterface.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OPCLIENTINTERFACE_H_
#define OPCLIENTINTERFACE_H_

#include "../../layer_network/include/Message.h"

/*
 *
 */
namespace optpaxos {

class ClientInterface {
  public:
    ClientInterface();
    virtual ~ClientInterface();

    virtual void handleMessage(netwrapper::Message* _msg) = 0;
};

}

#endif /* OPCLIENTINTERFACE_H_ */
