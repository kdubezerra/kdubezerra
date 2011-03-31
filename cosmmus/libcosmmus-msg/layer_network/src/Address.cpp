/*
 * Address.cpp
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Address.h"

using namespace netwrapper;

Address::Address() {
  address.host = 0;
  address.port = 0;
}

Address::Address(std::string _address, unsigned short _port) {
  SDLNet_ResolveHost(&address, _address.c_str(), (Uint16) _port);
}

Address::Address(Address* other) {
  this->address = other->address;
}

Address::~Address() {
  // TODO Auto-generated destructor stub
}

void Address::setAddress (IPaddress _address) {
  address = _address;
}

IPaddress Address::getAddress() {
  return address;
}

Message* Address::pack(Address* _addr) {
  Message* addrmsg = new Message();
  addrmsg->addInt((int) _addr->address.host);
  addrmsg->addInt((int) _addr->address.port);
  return addrmsg;
}

Address* Address::unpack(Message* _msg) {
  Address* addr = new Address();
  addr->address.host = (Uint32) _msg->getInt(0);
  addr->address.port = (Uint16) _msg->getInt(1);
  return addr;
}
