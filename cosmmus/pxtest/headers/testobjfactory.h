/*
 * testobjfactory.h
 *
 *  Created on: 22/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef TESTOBJFACTORY_H_
#define TESTOBJFACTORY_H_

#include <cosmmus-msg.h>

/*
 *
 */
class testobjfactory : public optpaxos::ObjectFactory {
  public:
    testobjfactory();
    virtual ~testobjfactory();

    optpaxos::Object* createObject();
    optpaxos::Object* copyObject(optpaxos::Object* _other);
    netwrapper::Message* packToNetwork(optpaxos::Object* _obj);
    optpaxos::Object* unpackFromNetwork(netwrapper::Message* _objMsg);
};

#endif /* TESTOBJFACTORY_H_ */
