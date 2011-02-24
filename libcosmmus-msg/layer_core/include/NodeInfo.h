/*
 * NodeInfo.h
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef NODEINFO_H_
#define NODEINFO_H_

#include "../../layer_network/include/GenericNode.h"
#include "../../layer_network/include/Message.h"

/*
 *
 */
namespace optpaxos {

enum NodeType { CLIENT_NODE , SERVER_NODE };

class NodeInfo : public netwrapper::GenericNode {
  public:
    NodeInfo();
    NodeInfo(int _id, NodeType _type, netwrapper::Address* _address); /// (!) deep copy of _address
    NodeInfo(NodeInfo* other);
    virtual ~NodeInfo();

    int getNodeId() const;
    void setNodeId(int _nodeId);

    void setNodeType(NodeType _nodeType);
    NodeType getNodeType() const;

    void setAddress(netwrapper::Address* _address);
    netwrapper::Address* getAdress();

    static netwrapper::Message* packToNetwork(NodeInfo* _node);
    static netwrapper::Message* packListToNetwork(std::list<NodeInfo*> _nodeList);
    static NodeInfo* unpackFromNetwork(netwrapper::Message* _msg);
    static std::list<NodeInfo*> unpackListFromNetwork(netwrapper::Message* _msg);

  private:
    int nodeId;
    NodeType nodeType;
    netwrapper::Address* nodeAddress;
};

}

#endif /* NODEINFO_H_ */
