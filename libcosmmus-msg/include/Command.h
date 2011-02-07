/*
 * Command.h
 *
 *  Created on: 31/01/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <list>

class Object;
class Message;
class GameServer;

class Command {
  public:
    Command();
    virtual ~Command();

    void addTarget(Object* _obj);
    std::list<Object*> getTargetList();

    void addServer(GameServer* _server);
    std::list<GameServer*> getServerList();

    void setContent(Message* _content);
    Message* getContent();

    void setKnowsTargets(bool _knowsTargets);
    bool knowsTargets();

    void setOptimisticallyDeliverable(bool _isOpDeliverable);
    bool isOptimisticallyDeliverable();

    void setConservativelyDeliverable(bool _isConsDeliverable);
    bool isConservativelyDeliverable();

  private:
    std::list<Object*> targetList;
    std::list<GameServer*> serverList;
    Message* commandContent;
    bool withTargets;
    bool optimistic;
    bool conservative;
};

#endif /* COMMAND_H_ */
