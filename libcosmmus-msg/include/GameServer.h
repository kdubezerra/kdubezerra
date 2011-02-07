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

// API level classes
class Command;
class Message;
class Object;
class Player;
class Group;

class GameServer {
  public:
    GameServer();
    virtual ~GameServer();

    int init(unsigned int _port);
    list<Group*> findGroups(std::string _address);

    void subscribe (Player* _player, Object* _obj);
    void unsubscribe (Player* _player, Object* _obj);

    void addManagedObject(Object* _obj);
    void removeManagedObject(Object* _obj);

    void setObjectModel(Object* _objModel);

    /*!
     * \brief This method must be implemented in the subclass in order to retrieve, based
     *        on the subclass' logic, which objects are affected by the command _cmd.
     * \param _cmd The Command whose targets are still not known, leaving to the server
     *        the task of finding them.
     * \return The method must return a list of the objects affected by the command _cmd.
     */
    virtual std::list<Object*> getTargets(Command* _cmd);

    virtual int handleAppRequest(Message* _req);

  private:
    Object* objectModel;
};

#endif /* GAMESERVER_H_ */
