/*
 * Group.h
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef GROUP_H_
#define GROUP_H_

#include <list>

namespace optpaxos {

class Object;
class Server;

/*
 *
 */
class Group {
  public:
    Group();
    virtual ~Group();

    void addServer(Server* _server);
    void removeServer(Server* _server);
    std::list<Server*> getServerList();

    void setCoordinator(Server* _server);
    Server* getCoordinator();

    void addManagedObject(GameObject* _obj);
    void removeManagedObject(GameObject* _obj);
    std::list<Object*> getGroupObjects();

    static std::list<Group*> findGroups(std::string _brokerUrl, unsigned port);

  private:
    std::list<Server*> serverList;
    std::list<Object*> managedObjects;
    Server* groupCoordinator;
};

}

#endif /* GROUP_H_ */
