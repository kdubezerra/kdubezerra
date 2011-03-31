/*
 * testserver.cpp
 *
 *  Created on: 21/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>

#include "../headers/testserver.h"

using namespace std;

testserver::testserver() {
  // TODO Auto-generated constructor stub

}

testserver::~testserver() {
  // TODO Auto-generated destructor stub
}

void testserver::handleOptimisticDelivery(optpaxos::Command* _cmd) {
  cout << "Optimistically delivered command: " << _cmd->getContent()->getString(0) << endl;
}
