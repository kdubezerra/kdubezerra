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
class Server;

/*!
 * \class Command
 * \brief Generic command to be delivered in order.
 */
class Command {
  public:
    Command();
    virtual ~Command();

    void addTarget(Object* _obj);
    void setTargetList(std::list<Object*> _targetList);
    std::list<Object*> getTargetList();

    void addServer(Server* _server);
    std::list<Server*> getServerList();

    void setContent(netwrapper::Message* _content);
    netwrapper::Message* getContent();

    void setKnowsTargets(bool _knowsTargets);
    bool knowsTargets();

    void setOptimisticallyDeliverable(bool _isOpDeliverable);
    bool isOptimisticallyDeliverable();

    void setConservativelyDeliverable(bool _isConsDeliverable);
    bool isConservativelyDeliverable();

  private:
    std::list<Object*> targetList;
    std::list<Server*> serverList;
    netwrapper::Message* commandContent;
    bool withTargets;
    bool optimistic;
    bool conservative;

};

}

#endif /* COMMAND_H_ */
