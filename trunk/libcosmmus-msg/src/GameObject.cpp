/*
 * GameObject.cpp
 *
 *  Created on: 31/01/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/GameObject.h"

GameObject::GameObject() {
  // TODO Auto-generated constructor stub

}

GameObject::~GameObject() {
  // TODO Auto-generated destructor stub
}

void GameObject::setId(long _id) {
  id = _id;
}

long GameObject::getId() {
  return id;
}

std::map<long, GameObject*> GameObject::getObjectList() {
  return objectList;
}

/*!
 * \brief Retrieves an object among the objects held in the memory.
 * \param _id The unique id of the desired object.
 * \return The method returns a pointer to the object, or NULL if it was not found.
 */
GameObject* GameObject::getObjectById(long _id) {
  std::map<long, GameObject*>::iterator it = objectList.find(_id);
  if (it != objectList.end())
    return it->second;
  else
    return NULL;
}

void GameObject::subscribe (Player* _player) {
  subscriberList.push_back(_player);
}

void GameObject::unsubscribe (Player* _player) {
  subscriberList.remove(_player);
}

std::list<Player*> GameObject::getSubscribers() {
  return subscriberList;
}
