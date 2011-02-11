/*
 * NodeInfo.h
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef NODEINFO_H_
#define NODEINFO_H_

#include "../../layer_network/include/GenericNode.h"

/*
 *
 */
namespace optpaxos {

enum NodeType { CLIENT_NODE , SERVER_NODE };

class NodeInfo : public netwrapper::GenericNode {
  public:
    NodeInfo();
    virtual ~NodeInfo();

    int getNodeId() const;
    NodeType getNodeType() const;
    void setNodeId(int _nodeId);
    void setNodeType(NodeType _nodeType);

  private:
    NodeType nodeType;
    int nodeId;
};

}

#endif /* NODEINFO_H_ */
