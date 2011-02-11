/*
 * NodeInfo.cpp
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/NodeInfo.h"

using namespace optpaxos;

NodeInfo::NodeInfo() {
  // TODO Auto-generated constructor stub

}

NodeInfo::~NodeInfo() {
  // TODO Auto-generated destructor stub
}

int NodeInfo::getNodeId() const {
    return nodeId;
}

NodeType NodeInfo::getNodeType() const {
    return nodeType;
}

void NodeInfo::setNodeId(int _nodeId) {
    nodeId = _nodeId;
}

void NodeInfo::setNodeType(NodeType _nodeType) {
    nodeType = _nodeType;
}
