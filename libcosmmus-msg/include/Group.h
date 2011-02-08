/*
 * Group.h
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef GROUP_H_
#define GROUP_H_

namespace cosmmusmsg {

class GameServer;

/*
 *
 */
class Group {
  public:
    Group();
    virtual ~Group();

    void addServer(GameServer* _server);
    void removeServer(GameServer* _server);
    std::list<GameServer*> getServerList();

    void setManager(GameServer* _server);
    GameServer* getManager();

    void addManagedObject(GameObject* _obj);
    void removeManagedObject(GameObject* _obj);
    std::list<GameObject*> getGroupObjects();

  private:
    std::list<GameServer*> serverList;
    std::list<GameObject*> managedObjects;
    GameServer* groupManager;
};

}

#endif /* GROUP_H_ */
