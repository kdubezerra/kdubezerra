/*
 * NodeInfo.cpp
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/NodeInfo.h"

using namespace optpaxos;
using namespace netwrapper;

NodeInfo::NodeInfo() {
  // TODO Auto-generated constructor stub
}

NodeInfo::NodeInfo(NodeInfo* other) {
  this->address = new Address(other->address);
  this->nodeId = other->nodeId;
  this->nodeType = other->nodeType;
}

NodeInfo::~NodeInfo() {
  // TODO Auto-generated destructor stub
}

int NodeInfo::getNodeId() const {
  return nodeId;
}

void NodeInfo::setNodeId(int _nodeId) {
  nodeId = _nodeId;
}

NodeType NodeInfo::getNodeType() const {
  return nodeType;
}

void NodeInfo::setNodeType(NodeType _nodeType) {
  nodeType = _nodeType;
}

void NodeInfo::setAddress(Address* _address) {
  address = _address;
}

Address* NodeInfo::getAdress() {
  return address;
}

Message* NodeInfo::packToNetwork(NodeInfo* _node) {
  Message* niMsg = new Message();
  niMsg->addInt(_node->getNodeId());
  niMsg->addInt((int) _node->getNodeType());
  niMsg->addMessage(Address::pack(_node->address));
  return niMsg;
}

netwrapper::Message* NodeInfo::packListToNetwork(std::list<NodeInfo*> _nodeList) {
  Message* nlMsg = new Message();
  nlMsg->addInt((int) _nodeList.size());
  for (std::list<NodeInfo*>::iterator it = _nodeList.begin() ; it != _nodeList.end() ; it++) {
    nlMsg->addMessage(NodeInfo::packToNetwork(*it));
  }
  return nlMsg;
}

NodeInfo* NodeInfo::unpackFromNetwork(netwrapper::Message* _msg) {
  NodeInfo* ni = new NodeInfo();
  ni->setNodeId(_msg->getInt(0));
  ni->setNodeType((NodeType) _msg->getInt(1));
  ni->setAddress(Address::unpack(_msg->getMessage(0)));
  return ni;
}

std::list<NodeInfo*> NodeInfo::unpackListFromNetwork(netwrapper::Message* _msg) {
  std::list<NodeInfo*> nl;
  int count = _msg->getInt(0);
  for (int i = 0 ; i < count ; i++) {
    nl.push_back(NodeInfo::unpackFromNetwork(_msg->getMessage(i)));
  }
  return nl;
}
