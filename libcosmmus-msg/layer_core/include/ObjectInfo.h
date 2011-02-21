/*
 * ObjectInfo.h
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OBJECTINFO_H_
#define OBJECTINFO_H_

#include <map>

#include "../../layer_network/include/Message.h"

/*
 *
 */
namespace optpaxos {

class ObjectInfo {
  public:
    ObjectInfo();
    ObjectInfo(ObjectInfo* _other);
    virtual ~ObjectInfo();

    int getId();
    void setId(int _id);

    long getLastStamp();
    void setLastStamp(long _stamp);

    static netwrapper::Message* packToNetwork(ObjectInfo* _objInfo);
    static netwrapper::Message* packListToNetwork(std::list<ObjectInfo*> _objInfoList);
    static netwrapper::Message* packIndexToNetwork(std::map<int, ObjectInfo*> _objInfoIndex);
    static ObjectInfo* unpackFromNetwork(netwrapper::Message* _msg);
    static std::list<ObjectInfo*> unpackListFromNetwork(netwrapper::Message* _msg);
    static std::map<int, ObjectInfo*> unpackIndexFromNetwork(netwrapper::Message* _msg);

  private:
    int objectId;
    long lastStamp;
};

}

#endif /* OBJECTINFO_H_ */
