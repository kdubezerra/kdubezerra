/*
 * Object.cpp
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Object.h"

using namespace optpaxos;
using namespace netwrapper;

Object::Object() {
  // TODO Auto-generated constructor stub

}

Object::~Object() {
  // TODO Auto-generated destructor stub
}

int Object::getId() {
  return id;
}

void Object::setId(int _id) {
  id = _id;
}

Message* Object::packToNetwork(Object* _obj) {
  return objFactory->packToNetwork(_obj);
}

Message* Object::packObjectListToNetwork(std::list<Object*> _objList) {
  Message* objListMsg = new Message();

  objListMsg->addInt((int) _objList.size());
  for (std::list<Object*>::iterator it = _objList.begin() ; it != _objList.end() ; it++)
    objListMsg->addMessage(Object::packToNetwork(*it));

  return objListMsg;
}

Object* Object::unpackFromNetwork(netwrapper::Message* _msg) {
  return objFactory->unpackFromNetwork(_msg);
}

std::list<Object*> Object::unpackObjectListFromNetwork(netwrapper::Message* _msg) {
  std::list<Object*> objList;

  int objectCount = _msg->getInt(0);
  for (int index = 0 ; index < objectCount ; index++)
    objList.push_back(Object::unpackFromNetwork(_msg->getMessage(index)));

  return objList;
}
