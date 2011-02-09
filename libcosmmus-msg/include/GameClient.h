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

namespace cosmmusmsg {

// API layer classes
class GameCommand;
class GameObject;

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
    void submitCommand(GameCommand* _cmd);
    //int submitRequest(OPMessage* _request);

    void setObjectModel(GameObject* _objModel);
    std::list<GameObject*> getObjectList();

    /*!
     * \brief Abstract method to be implement by the application. It is called when a message from the server is received.
     * \param _msg The message object received from the server.
     * \return An integer number whose meaning should be defined by the application. 0 should mean message successfully handled.
     */
    virtual int handleServerAppMsg(Message* _msg);

  private:
    std::list<GameObject*> objList;
    Client* coreClient;
    GameObject* objectModel;
};

}

#endif /* GAMECLIENT_H_ */
