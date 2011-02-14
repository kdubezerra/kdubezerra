/*
 * GenericNode.h
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef GENERICNODE_H_
#define GENERICNODE_H_

#include "../include/Address.h"

/*
 *
 */
namespace netwrapper {

class GenericNode {
  public:
    GenericNode();
    virtual ~GenericNode();

    Address* getAddress();
    void setAddress (Address* address);

  private:
    Address* address;
};

}

#endif /* GENERICNODE_H_ */
