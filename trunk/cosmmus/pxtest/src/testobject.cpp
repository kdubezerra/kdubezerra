/*
 * testobject.cpp
 *
 *  Created on: 22/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>
#include "../headers/testobject.h"

using namespace std;
using namespace optpaxos;

testobject::testobject() {
  optState = 0;
  conState = 0;
}

testobject::testobject(testobject* _other) : optpaxos::Object(_other) {
  optState = _other->optState;
  conState = _other->conState;
}

testobject::testobject(int _id) : Object(_id) {
  optState = 0;
  conState = 0;
}

testobject::~testobject() {
  // TODO Auto-generated destructor stub
}

void testobject::handleNewOptimisticState(Object* _state) {
  cout << "Optimistic state with timestamp " << _state->getInfo()->getLastStamp() << "delivered for object " << _state->getInfo()->getId() << endl;
}

void testobject::handleNewConservativeState(Object* _state) {
  cout << "Conservative state with timestamp " << _state->getInfo()->getLastStamp() << "delivered for object " << _state->getInfo()->getId() << endl;
}

void testobject::handleOptimisticDelivery(Command* _cmd) {
  //cout << "testobject::handleOptimisticDelivery: command " << _cmd->getContent()->getString(0) << "(" << _cmd->getContent()->getInt(0) << ") OPTIMISTICALLY delivered for object " << this->getInfo()->getId() << endl;
  if (_cmd->getContent()->getString(0).compare("ADD") == 0)
    optState += _cmd->getContent()->getInt(0);
  if (_cmd->getContent()->getString(0).compare("MUL") == 0)
    optState *= _cmd->getContent()->getInt(0);
}

void testobject::handleConservativeDelivery(Command* _cmd) {
  //cout << "testobject::handleConservativeDelivery: command " << _cmd->getId() << " CONSERVATIVELY delivered for object " << this->getInfo()->getId() << endl;
  if (_cmd->getContent()->getString(0).compare("ADD") == 0)
    conState += _cmd->getContent()->getInt(0);
  if (_cmd->getContent()->getString(0).compare("MUL") == 0)
    conState *= _cmd->getContent()->getInt(0);
}
