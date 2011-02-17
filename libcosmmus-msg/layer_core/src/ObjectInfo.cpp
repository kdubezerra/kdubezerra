/*
 * ObjectInfo.cpp
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/ObjectInfo.h"

using namespace optpaxos;

ObjectInfo::ObjectInfo() {
  // TODO Auto-generated constructor stub

}

ObjectInfo::~ObjectInfo() {
  // TODO Auto-generated destructor stub
}

int ObjectInfo::getId() {
  return objectId;
}

void ObjectInfo::setId(int _id) {
  objectId = _id;
}

long ObjectInfo::getLastStamp() {
  return lastStamp;
}

void ObjectInfo::setLastStamp(long _stamp) {
  lastStamp = _stamp;
}
