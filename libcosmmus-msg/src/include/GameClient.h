/*
 * GameClient.h
 *
 *  Created on: 03/02/2011
 *      Author: Eduardo
 */

#ifndef GAMECLIENT_H_
#define GAMECLIENT_H_

#include <string>
#include <list>

class Command;
class Object;
class GameServer;

class GameClient {
  public:
    GameClient();
    virtual ~GameClient();
    int connect(std::string _address);
    int disconnect();
    int submitCommand(Command* _cmd, std::list<Object*> _objs);
    int submitCommand(Command* _cmd, std::list<GameServer*> _servers);
    int submitRequest(std::string _request);
    virtual int handleServerMsg(std::string _msg);

  protected:
    int submitCommand(Command* _cmd, std::list<GameServer*> _servers, bool _knowsObjects);
};

#endif /* GAMECLIENT_H_ */
