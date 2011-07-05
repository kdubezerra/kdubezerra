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

enum CommandStage {
  PROPOSING_LOCAL,
  GLOBAL_STAMPING,
  UPDATING_CLOCK,
  DELIVERABLE
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
    Command(long _id);
    Command(Command* _cmd);
    virtual ~Command();
    bool equals(Command* _other);
    static bool compareLogicalStampThenId(Command* c1, Command* c2);
    static bool compareTimeStampThenId(Command* c1, Command* c2);
    static bool compareLSthenTSthenID(Command* c1, Command* c2);
    static bool compareId(Command* c1, Command* c2);
    static bool hasSameId(Command* c1, Command* c2);
    static void removeDuplicates(std::list<Command*>& _cmdlist);

    void addTarget(ObjectInfo* _obj);
    void addTargetList(std::list<ObjectInfo*> _targetList);
    std::list<ObjectInfo*> getTargetList();

    void addPriorState(Object* _state);
    void addPriorStateList(std::list<Object*> _stateList);
    std::list<Object*> getPriorStateList();

    void addGroup(Group* _server);
    void addGroupList(std::list<Group*> _serverList);
    void findGroups();
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

    void calcLogicalStamp(Group* _localGroup = NULL);
    long getLogicalStamp();
    void setLogicalStamp(long _stamp);
    long getTimeStamp();
    void setTimeStamp(long _stamp);

    CommandStage getStage();
    void setStage(CommandStage _stage);

    long getId();
    void setId(long _id);

    static netwrapper::Message* packToNetwork(Command* _cmd);
    static netwrapper::Message* packCommandListToNetwork(std::list<Command*> _cmdList);
    static Command* unpackFromNetwork(netwrapper::Message* _msg);
    static std::list<Command*> unpackCommandListFromNetwork(netwrapper::Message* _msg);

  private:
    std::list<ObjectInfo*> targetList;
    std::list<Object*> priorStateList;
    std::list<Group*> groupList;
    netwrapper::Message* commandContent;
    bool withTargets;
    bool withPriorStates;
    bool withGroups;
    bool withContent;
    bool optimistic;
    bool conservative;
    long logicalStamp;
    long timeStamp;
    long commandId;
    CommandStage stage;
};

}

#endif /* COMMAND_H_ */
