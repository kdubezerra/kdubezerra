/*
 * testclient.cpp
 *
 *  Created on: 01/04/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../headers/testclient.h"

testclient::testclient() {

  // TODO Auto-generated constructor stub

}

testclient::~testclient() {
  // TODO Auto-generated destructor stub
}

void testclient::handleMessage(netwrapper::Message* _msg) {
}

void testclient::sendCommand(Command* cmd, long _clSeq, int _clId) {
  cmd->setId(_clSeq * 10 + _clId);
  submitCommand(cmd);
}
