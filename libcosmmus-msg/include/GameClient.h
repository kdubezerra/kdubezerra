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

// API layer classes
class Command;
class Message;
class Object;

// core layer classes
class CoreClient;

/*!
 * \class GameClient
 * \brief Class to be extended by the application in order to connect to the game server.
 */
class GameClient {
  public:
    GameClient();
    virtual ~GameClient();

    int connect(std::string _address);
    int disconnect();
    int submitCommand(Command* _cmd);
    int submitRequest(Message* _request);
    virtual int handleServerAppMsg(Message* _msg);

    std::list<Object*> getObjectList();

    virtual Object* newAppObject()=0;

  private:
    std::list<Object*> objList;
    CoreClient* coreClient;
};

#endif /* GAMECLIENT_H_ */
