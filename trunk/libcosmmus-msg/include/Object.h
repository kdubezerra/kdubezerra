/*
 * Object.h
 *
 *  Created on: 31/01/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <list>
#include <map>

class Command;
class Message;

class Object {
  public:
    Object();
    virtual ~Object();

    void setId(long _id);
    long getId();

    static std::map<long,Object*> getObjectList();
    static Object* getObjectById(long _id);

    //These methods should be overridden by the application
    virtual void handleNewOptimisticState(Object* _state) = 0;
    virtual void handleNewConservativeState(Object* _state) = 0;

    virtual void handleOptimisticDelivery(Message* _cmd) = 0;
    virtual void handleConservativeDelivery(Message* _cmd) = 0;

    /*!
    * \brief As the library needs to instantiate application objects, whose type is not
    *        known by it, it must use a callback function, delegating the task of instantiating
    *        objects to the application developer.
    * \return It returns a pointer to the newly instantiated application's extended object.
    */
    virtual Object* createNew() = 0;

  private:
    long id;
    static std::map<long, Object*> objectList;
};

#endif /* OBJECT_H_ */
