/*
 * ObjectInfo.h
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OBJECTINFO_H_
#define OBJECTINFO_H_

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

    static netwrapper::Message* packObjectInfoListToNetwork(std::list<ObjectInfo*> _objInfoList);
    static std::list<ObjectInfo*> unpackObjectInfoListFromNetwork(netwrapper::Message* _msg);

  private:
    int objectId;
};

}

#endif /* OBJECTINFO_H_ */
