/*
 * GameCommand.h
 *
 *  Created on: 31/01/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef GAMECOMMAND_H_
#define GAMECOMMAND_H_

#include <list>
#include "../layer_core/include/Command.h"

namespace cosmmusmsg {

// API level classes
class GameObject;
class Message;
class GameServer;

class GameCommand : public optpaxos::Command {
  public:
    GameCommand();
    virtual ~GameCommand();
};

}

#endif /* GAMECOMMAND_H_ */
