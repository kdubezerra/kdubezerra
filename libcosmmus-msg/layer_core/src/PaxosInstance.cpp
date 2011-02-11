/*
 * PaxosInstance.cpp
 *
 *  Created on: 10/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/PaxosInstance.h"

namespace optpaxos {

PaxosInstance::PaxosInstance() {
  acceptedValue = NULL;
  learnt = false;
}

PaxosInstance::~PaxosInstance() {
  // TODO Auto-generated destructor stub
}

}
