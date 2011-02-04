/*
 * Object.h
 *
 *  Created on: 31/01/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OBJECT_H_
#define OBJECT_H_

class Object {
  public:
    Object();
    virtual ~Object();

    int setId(long _id);
    long getId();

    //These methods should be overridden by the application
    virtual void handleOptimisticDelivery();
    virtual void handleConservativeDelivery();

    virtual Object* createNew()=0;

  private:
    long id;
};

#endif /* OBJECT_H_ */
