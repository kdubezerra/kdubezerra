/*
 * Group.h
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef GROUP_H_
#define GROUP_H_

#include <list>
#include <map>

#include "../../layer_network/include/Message.h"

namespace optpaxos {

class NodeInfo;
class ObjectInfo;
class Server;

/*
 *
 */
class Group {
  public:
    Group();
    Group(int _id);
    Group(Group* _group);
    virtual ~Group();

    void setId(int _id);
    int getId();

    void addServer(NodeInfo* _server);
    void removeServer(NodeInfo* _server);
    std::list<NodeInfo*> getServerList();

    void setCoordinator(NodeInfo* _server);
    NodeInfo* getCoordinator();

    void addManagedObject(ObjectInfo* _obj);
    bool hasObject(ObjectInfo* _obj);
    void removeManagedObject(ObjectInfo* _obj);
    std::map<int, ObjectInfo*> getObjectsIndex();
    std::list<ObjectInfo*> getObjectsList();

    static void addGroup(Group* _grp);
    static std::list<Group*> requestGroupsList(std::string _brokerUrl, unsigned port);
    static std::list<Group*> getGroupsList();

    static netwrapper::Message* packToNetwork(Group* _group);
    static netwrapper::Message* packListToNetwork(std::list<Group*> _groupList);
    static Group* unpackFromNetwork(netwrapper::Message* _msg);
    static std::list<Group*> unpackListFromNetwork(netwrapper::Message* _msg);

  private:
    int id;
    std::list<NodeInfo*> serverList;
    NodeInfo* groupCoordinator;
    std::map<int, ObjectInfo*> managedObjects;
    static std::list<Group*> allGroupsList;
};

}

#endif /* GROUP_H_ */
