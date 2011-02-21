/*
 * ObjectInfo.cpp
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/ObjectInfo.h"

using namespace optpaxos;
using namespace netwrapper;

ObjectInfo::ObjectInfo() {
  // TODO Auto-generated constructor stub

}

ObjectInfo::ObjectInfo(ObjectInfo* other) {
  this->lastStamp = other->lastStamp;
  this->objectId = other->objectId;
}

ObjectInfo::~ObjectInfo() {
  // TODO Auto-generated destructor stub
}

int ObjectInfo::getId() {
  return objectId;
}

void ObjectInfo::setId(int _id) {
  objectId = _id;
}

long ObjectInfo::getLastStamp() {
  return lastStamp;
}

void ObjectInfo::setLastStamp(long _stamp) {
  lastStamp = _stamp;
}

Message* ObjectInfo::packToNetwork(ObjectInfo* _objInfo) {
  Message* objMsg = new Message();
  objMsg->addInt(_objInfo->getId());
  objMsg->addInt(_objInfo->getLastStamp());
  return objMsg;
}

Message* ObjectInfo::packListToNetwork(std::list<ObjectInfo*> _objInfoList) {
  Message* objInfoListMsg = new Message();
  objInfoListMsg->addInt((int) _objInfoList.size());
  for (std::list<ObjectInfo*>::iterator it = _objInfoList.begin() ; it != _objInfoList.end() ; it++)
    objInfoListMsg->addMessage(ObjectInfo::packToNetwork(*it));
  return objInfoListMsg;
}

Message* ObjectInfo::packIndexToNetwork(std::map<int, ObjectInfo*> _objInfoIndex) {
  Message* objInfoIndexMsg = new Message();
  objInfoIndexMsg->addInt((int) _objInfoIndex.size());
  for (std::map<int, ObjectInfo*>::iterator it = _objInfoIndex.begin() ; it != _objInfoIndex.end() ; it++)
    objInfoIndexMsg->addMessage(ObjectInfo::packToNetwork(it->second));
  return objInfoIndexMsg;
}

ObjectInfo* ObjectInfo::unpackFromNetwork(Message* _msg) {
  ObjectInfo* rcvdObjInfo = new ObjectInfo();
  rcvdObjInfo->setId(_msg->getInt(0));
  rcvdObjInfo->setLastStamp(_msg->getInt(1));
  return rcvdObjInfo;
}

std::list<ObjectInfo*> ObjectInfo::unpackListFromNetwork(Message* _msg) {
  std::list<ObjectInfo*> objInfoList;
  int objInfoCount = _msg->getInt(0);
  for (int i = 0 ; i < objInfoCount ; i++) {
    objInfoList.push_back(ObjectInfo::unpackFromNetwork(_msg->getMessage(i)));
  }
  return objInfoList;
}

std::map<int, ObjectInfo*> ObjectInfo::unpackIndexFromNetwork(Message* _msg) {
  std::map<int, ObjectInfo*> objInfoIndex;
  int objInfoCount = _msg->getInt(0);
  for (int i = 0 ; i < objInfoCount ; i++) {
    ObjectInfo* objInfo = ObjectInfo::unpackFromNetwork(_msg->getMessage(i));
    objInfoIndex[objInfo->getId()] = objInfo;
  }
  return objInfoIndex;
}
