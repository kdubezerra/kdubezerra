/*
 * GameServer.h
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef GAMESERVER_H_
#define GAMESERVER_H_

#include <list>
#include <string>

#include "../layer_core/include/Group.h"
#include "../layer_core/include/Server.h"
#include "../layer_core/include/ServerInterface.h"
#include "../layer_network/include/Message.h"


namespace cosmmusmsg {

// API level classes
class GameCommand;
class GameObject;
class Player;

class GameServer : public optpaxos::ServerInterface {
  public:
    GameServer();
    virtual ~GameServer();

    int init(unsigned int _port);

    std::list<optpaxos::Group*> findGroups(std::string _address, int port);
    int joinServerGroup(optpaxos::Group* _group);
    void leaveServerGroup();

    void setObjectModel(GameObject* _objModel);

    /*!
     * \brief This method must be implemented in the subclass in order to retrieve, based
     *        on the application's logic, which objects are affected by the command _cmd.
     * \param _cmd The GameCommand whose targets are still not known, leaving to the server
     *        the task of finding them.
     * \return The method must return a list of the objects affected by the command _cmd.
     */
    virtual std::list<GameObject*> getTargets(GameCommand* _cmd);

    /*!
     * \brief This method must be implemented in the subclass in order to process application
     *        level requests sent by a client.
     * \param _req The message sent by the client.
     * \return The application may define a set of return values in order to inform the result
     *         of the request processing.
     */
    virtual int handleAppRequest(netwrapper::Message* _req);

  private:
    GameObject* objectModel;
    optpaxos::Group* serverGroup;
    optpaxos::Server* coreServer;
};

}

#endif /* GAMESERVER_H_ */
