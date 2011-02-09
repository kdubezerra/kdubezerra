/*
 * NodeInfo.cpp
 *
 *  Created on: 09/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/NodeInfo.h"

using namespace optpaxos;
using namespace netwrapper;

NodeInfo::NodeInfo() {
  // TODO Auto-generated constructor stub

}

NodeInfo::~NodeInfo() {
  // TODO Auto-generated destructor stub
}

std::string NodeInfo::getAddress() const {
  return address;
}

int NodeInfo::getId() const {
  return id;
}

unsigned NodeInfo::getPort() const {
  return port;
}

void NodeInfo::setAddress(std::string _address) {
  address = _address;
}

void NodeInfo::setId(int _id) {
  id = _id;
}

void NodeInfo::setPort(unsigned  _port) {
  port = _port;
}

Message* NodeInfo::packToNetwork(NodeInfo* _node) {
  Message* nodeMsg = new Message();

  nodeMsg->addInt(_node->getId());
  nodeMsg->addString(_node->getAddress());
  nodeMsg->addString(_node->getPort());

  return nodeMsg;
}

Message* NodeInfo::packNodeListToNetwork(std::list<NodeInfo*> _nodeList) {
  Message* nodeListMsg = new Message();

  nodeListMsg->addInt((int) _nodeList.size());
  for (std::list<NodeInfo*>::iterator it = _nodeList.begin() ; it != _nodeList.end() ; it++) {
    nodeListMsg->addMessage(NodeInfo::packToNetwork(*it));
  }

  return nodeListMsg;
}

NodeInfo* NodeInfo::unpackFromNetwork(Message* _msg) {
  NodeInfo* node = new NodeInfo();

  node->setId(_msg->getInt(0));
  node->setAddress(_msg->getString(0));
  node->setPort(_msg->getInt(1));

  return node;
}

std::list<NodeInfo*> NodeInfo::unpackNodeListFromNetwork(Message* _msg) {
  std::list<NodeInfo*> nodeList;

  int nodeCount = _msg->getInt(0);
  for (int index = 0 ; index < nodeCount ; index++) {
    nodeList.push_back(NodeInfo::unpackFromNetwork(_msg->getMessage(index)));
  }

  return nodeList;
}
