/*
 * GenericNode.cpp
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/GenericNode.h"

using namespace std;
using namespace netwrapper;

GenericNode::GenericNode() {
  // TODO Auto-generated constructor stub
}

GenericNode::~GenericNode() {
  // TODO Auto-generated destructor stub
}

Address* GenericNode::getAddress() {
  return address;
}

void GenericNode::setAddress (Address* _address) {
  address = _address;
}
