/*
 * Object.h
 *
 *  Created on: 31/01/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch Bezerra
 */

#ifndef OBJECT_H_
#define OBJECT_H_

class Object {
  public:
    Object();
    virtual ~Object();

    //These methods should be overridden by the application
    virtual void handleOptimisticDelivery();
    virtual void handleConservativeDelivery();
};

#endif /* OBJECT_H_ */
