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
    void submitCommand(Command* _cmd);
    int submitRequest(Message* _request);
    void setObjectModel(Object* _objModel);
    std::list<Object*> getObjectList();

    /*!
     * \brief Abstract method to be implement by the application. It is called when a message from the server is received.
     * \param _msg The message object received from the server.
     * \return An integer number whose meaning should be defined by the application. 0 should mean message successfully handled.
     */
    virtual int handleServerAppMsg(Message* _msg);

  private:
    std::list<Object*> objList;
    CoreClient* coreClient;
    Object* objectModel;
};

#endif /* GAMECLIENT_H_ */
