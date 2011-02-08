/*
 * ServerMessage.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/ServerMessage.h"

using namespace optpaxos;

ServerMessage::ServerMessage() {
  // TODO Auto-generated constructor stub

}

ServerMessage::~ServerMessage() {
  for (std::list<Command*>::iterator it = commandList.begin ; it != commandList.end() ; it++)
    delete *it;

  for (std::list<Object*>::iterator it = stateList.begin() ; it != stateList.end() ; it++)
    delete *it;

  delete extraPayload;
}
