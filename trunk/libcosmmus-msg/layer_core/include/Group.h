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

class ObjectInfo;
class Server;

/*
 *
 */
class Group {
  public:
    Group();
    Group(Group* _group);
    virtual ~Group();

    void addServer(NodeInfo* _server);
    void removeServer(NodeInfo* _server);
    std::list<NodeInfo*> getServerList();

    void setCoordinator(NodeInfo* _server);
    NodeInfo* getCoordinator();

    void addManagedObject(ObjectInfo* _obj);
    void removeManagedObject(ObjectInfo* _obj);
    std::list<ObjectInfo*> getGroupObjects();

    static std::list<Group*> findGroups(std::string _brokerUrl, unsigned port);

    static netwrapper::Message* packToNetwork(Group* _obj);
    static netwrapper::Message* packGroupListToNetwork(std::list<Group*> _objList);
    static Group* unpackFromNetwork(netwrapper::Message* _msg);
    static std::list<Group*> unpackGroupListFromNetwork(netwrapper::Message* _msg);

  private:
    std::list<NodeInfo*> serverList;
    std::list<ObjectInfo*> managedObjects;
    NodeInfo* groupCoordinator;
};

}

#endif /* GROUP_H_ */
