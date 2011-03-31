/*
 * testobjfactory.cpp
 *
 *  Created on: 22/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../headers/testobjfactory.h"
#include "../headers/testobject.h"

using namespace optpaxos;
using namespace netwrapper;

testobjfactory::testobjfactory() {
  // TODO Auto-generated constructor stub
}

testobjfactory::~testobjfactory() {
  // TODO Auto-generated destructor stub
}

Object* testobjfactory::createObject() {
  return new testobject();
}

Object* testobjfactory::copyObject(Object* _other) {
  return new testobject((testobject*) _other);
}

netwrapper::Message* testobjfactory::packToNetwork(Object* _obj) {
  testobject* tobj = (testobject*) _obj;
  Message* msg = new Message();
  msg->addInt(tobj->optState);
  msg->addInt(tobj->conState);
  return msg;
}

Object* testobjfactory::unpackFromNetwork(netwrapper::Message* _objMsg) {
  testobject* tobj = new testobject();
  tobj->optState = _objMsg->getInt(0);
  tobj->conState = _objMsg->getInt(1);
  return tobj;
}
