/*
 * Command.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <list>
#include "../../layer_network/include/Message.h"

namespace optpaxos {

class Object;
class NodeInfo;

/*!
 * \class Command
 * \brief Generic command to be delivered in order.
 */
class Command : private OPTPaxosControl {
  public:
    Command();
    virtual ~Command();

    void addTarget(Object* _obj);
    void setTargetList(std::list<Object*> _targetList);
    std::list<Object*> getTargetList();

    void addServer(NodeInfo* _server);
    void setServerList(std::list<NodeInfo*> _serverList);
    std::list<NodeInfo*> getServerList();

    void setContent(netwrapper::Message* _content);
    netwrapper::Message* getContent();

    void setKnowsTargets(bool _knowsTargets);
    bool knowsTargets();

    void setKnowsServers(bool _knowsServers);
    bool knowsServers();

    bool hasContent();

    void setOptimisticallyDeliverable(bool _isOpDeliverable);
    bool isOptimisticallyDeliverable();

    void setConservativelyDeliverable(bool _isConsDeliverable);
    bool isConservativelyDeliverable();

    static netwrapper::Message* packToNetwork(Command* _cmd);
    static netwrapper::Message* packCommandListToNetwork(std::list<Command*> _cmdList);
    static Command* unpackFromNetwork(netwrapper::Message* _msg);
    static std::list<Command*> unpackCommandListFromNetwork(netwrapper::Message* _msg);

  private:
    std::list<Object*> targetList;
    std::list<NodeInfo*> serverList;
    netwrapper::Message* commandContent;
    bool withTargets;
    bool withServers;
    bool withContent;
    bool optimistic;
    bool conservative;
};

}

#endif /* COMMAND_H_ */
