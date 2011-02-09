/*
 * OPTPaxosControl.cpp
 *
 *  Created on: 09/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/OPTPaxosControl.h"

using namespace optpaxos;

ObjectFactory* OPTPaxosControl::objFactory = NULL;

OPTPaxosControl::OPTPaxosControl() {
  // TODO Auto-generated constructor stub

}

OPTPaxosControl::~OPTPaxosControl() {
  // TODO Auto-generated destructor stub
}

void OPTPaxosControl::setObjectFactory(ObjectFactory* _factory) {
  objFactory = _factory;
}
