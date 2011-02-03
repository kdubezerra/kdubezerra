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
class GameServer;

class Command {
  public:
    Command();
    virtual ~Command();

  protected:
    std::list<Object*> targetList;
    std::list<GameServer*> serverList;
    bool knowsTargets;
    // if the command contains the list of target objects, the corresponding servers must be found
    // if not, it should know the list of possibly
};

#endif /* COMMAND_H_ */
