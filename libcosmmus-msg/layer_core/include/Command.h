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

enum CommandType {
  OPTIMISTIC,
  CONSERVATIVE
};

class Group;
class Object;
class ObjectInfo;

/*!
 * \class Command
 * \brief Generic command to be delivered in order.
 */
class Command {
  public:
    Command();
    Command(Command* _cmd);
    virtual ~Command();
    bool equals(Command* _other);
    static bool compareLT(Command* c1, Command* c2);

    void addTarget(ObjectInfo* _obj);
    void setTargetList(std::list<ObjectInfo*> _targetList);
    std::list<ObjectInfo*> getTargetList();

    void addPriorStateState(Object* _state);
    void setPriorStateList(std::list<Object*> _stateList);
    std::list<Object*> getPriorStateList();

    void addGroup(Group* _server);
    void setGroupList(std::list<Group*> _serverList);
    std::list<Group*> findGroups(); // TODO:
    std::list<Group*> getGroupList();

    void setContent(netwrapper::Message* _content);
    netwrapper::Message* getContent();

    void setKnowsTargets(bool _knowsTargets);
    bool knowsTargets();

    void setHasPriorStates(bool _hasPriorStates);
    bool hasPriorStates();

    void setKnowsGroups(bool _knowsGroups);
    bool knowsGroups();

    bool hasContent();

    void setOptimisticallyDeliverable(bool _isOpDeliverable);
    bool isOptimisticallyDeliverable();

    void setConservativelyDeliverable(bool _isConsDeliverable);
    bool isConservativelyDeliverable();

    void calculateStamp();
    long getStamp();
    void setStamp(long _stamp);

    static netwrapper::Message* packToNetwork(Command* _cmd);
    static netwrapper::Message* packCommandListToNetwork(std::list<Command*> _cmdList);
    static Command* unpackFromNetwork(netwrapper::Message* _msg);
    static std::list<Command*> unpackCommandListFromNetwork(netwrapper::Message* _msg);

  private:
    std::list<ObjectInfo*> targetList;
    std::list<Object*> necessaryStates;
    std::list<Group*> groupList;
    netwrapper::Message* commandContent;
    bool withTargets;
    bool withPriorStates;
    bool withGroups;
    bool withContent;
    bool optimistic;
    bool conservative;
    long stamp;
};

}

#endif /* COMMAND_H_ */
