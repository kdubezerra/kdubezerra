/*
 * Object.cpp
 *
 *  Created on: 31/01/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Object.h"

Object::Object() {
  // TODO Auto-generated constructor stub

}

Object::~Object() {
  // TODO Auto-generated destructor stub
}

void Object::setId(long _id) {
  id = _id;
}

long Object::getId() {
  return id;
}

std::map<long, Object*> Object::getObjectList() {
  return objectList;
}

/*!
 * \brief Retrieves an object among the objects held in the memory.
 * \param _id The unique id of the desired object.
 * \return The method returns a pointer to the object, or NULL if it was not found.
 */
Object* Object::getObjectById(long _id) {
  std::map<long, Object*>::iterator it = objectList.find(_id);
  if (it != objectList.end())
    return it->second;
  else
    return NULL;
}
