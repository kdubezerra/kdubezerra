/*
 * ObjectFactory.h
 *
 *  Created on: 09/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OBJECTFACTORY_H_
#define OBJECTFACTORY_H_

#include <list>

/*
 *
 */
namespace optpaxos {

class Object;

class ObjectFactory {
  public:
    ObjectFactory();
    virtual ~ObjectFactory();

    virtual netwrapper::Message* packToNetwork(Object* _obj) = 0;
    virtual Object* unpackFromNetwork(netwrapper::Message* _msg) = 0;

    /*!
    * \brief As the library needs to instantiate application objects, whose type is not
    *        known by it, it must use a callback function, delegating the task of instantiating
    *        objects to the application developer.
    * \return It returns a pointer to the newly instantiated application's extended object.
    */
    virtual Object* createObject() = 0;

    /*!
     * \brief As the library has no knowledge of what the application's object states
     *        consists of, the developer must do, himself, the packing of the object
     *        into a netwrapper::Message object to be sent through the network.
     * \return Returns a pointer to a netwrapper::Message object, ready to be sent by
     *         the client.
     */
    virtual netwrapper::Message* packToNetwork(Object* _obj) = 0;

    /*!
     * \brief Upon receiving a packed object as a message from the network, it must be unpacked.
     *        As the library does not know beforehand what kind of object that is - and what are
     *        its attributes, the developer must do it, by implementing the unpackFromNetwork
     *        method of the ObjectFactory.
     * \param _objMsg The message from the network, containing the object.
     * \return Returns a pointer to the newly created Object, containing the unpacked data from the
     *         network message.
     */
    virtual Object* unpackFromNetwork(netwrapper::Message* _objMsg);


};

}

#endif /* OBJECTFACTORY_H_ */
