/*
 * FIFOReliableServer.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef FIFORELIABLESERVER_H_
#define FIFORELIABLESERVER_H_

#include "GenericNode.h"

/*
 *
 */
namespace netwrapper {

class FIFOReliableServer : public GenericNode {
  public:
    FIFOReliableServer();
    virtual ~FIFOReliableServer();

    int init (unsigned _port);
};

}

#endif /* FIFORELIABLESERVER_H_ */
