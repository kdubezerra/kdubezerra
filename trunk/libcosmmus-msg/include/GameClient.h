/*
 * GameClient.h
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
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
    int submitCommand(Command* _cmd);
    int submitRequest(std::string _request);
    virtual int handleServerMsg(std::string _msg);

};

#endif /* GAMECLIENT_H_ */
