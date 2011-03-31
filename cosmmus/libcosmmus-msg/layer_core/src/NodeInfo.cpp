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
  nodeAddress = NULL;
  nodeId = -1;
  nodeType = SERVER_NODE;
}

NodeInfo::NodeInfo(int _id, NodeType _type, Address* _address) {
  nodeId = _id;
  nodeType = _type;
  nodeAddress = new Address(_address);
}

NodeInfo::NodeInfo(NodeInfo* other) {
  this->nodeAddress = new Address(other->nodeAddress);
  this->nodeId = other->nodeId;
  this->nodeType = other->nodeType;
}

NodeInfo::~NodeInfo() {
  if (nodeAddress != NULL)
    delete nodeAddress;
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
  nodeAddress = new Address(_address);
}

Address* NodeInfo::getAdress() {
  return nodeAddress;
}

Message* NodeInfo::packToNetwork(NodeInfo* _node) {
  Message* niMsg = new Message();
  niMsg->addInt(_node->nodeId);
  niMsg->addInt((int) _node->nodeType);
  niMsg->addMessage(Address::pack(_node->nodeAddress));
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
  ni->nodeId = _msg->getInt(0);
  ni->nodeType = (NodeType) _msg->getInt(1);
  ni->nodeAddress = Address::unpack(_msg->getMessage(0));
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
