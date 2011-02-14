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

#include "../layer_core/include/Client.h"
#include "../layer_core/include/ClientInterface.h"
#include "../layer_core/include/Command.h"

#include "../layer_network/include/Message.h"

namespace cosmmusmsg {

// API layer classes
class GameCommand;
class GameObject;

/*!
 * \class GameClient
 * \brief Class to be extended by the application in order to connect to the game server.
 */
class GameClient : public optpaxos::ClientInterface {
  public:
    GameClient();
    virtual ~GameClient();

    int connect(std::string _address, unsigned port);
    int disconnect();
    void submitCommand(optpaxos::Command* _cmd);
    int submitRequest(netwrapper::Message* _request);

    void setObjectModel(GameObject* _objModel);
    std::list<GameObject*> getObjectList();

    /*!
     * \brief Abstract method to be implement by the application. It is called when a message from the server is received.
     * \param _msg The message object received from the server.
     * \return An integer number whose meaning should be defined by the application. 0 should mean message successfully handled.
     */
    virtual int handleServerAppMsg(netwrapper::Message* _msg);

  private:
    std::list<GameObject*> objList;
    optpaxos::Client* coreClient;
    GameObject* objectModel;
};

}

#endif /* GAMECLIENT_H_ */
