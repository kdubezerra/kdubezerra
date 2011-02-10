/*
 * Object.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <list>
#include "../../layer_network/include/Message.h"

namespace optpaxos {

/*
 *
 */
class Object : private OPTPaxosControl {
  public:
    Object();
    virtual ~Object();

    int getId();
    void setId(int _id);

    static netwrapper::Message* packToNetwork(Object* _obj);
    static netwrapper::Message* packObjectListToNetwork(std::list<Object*> _objList);
    static Object* unpackFromNetwork(netwrapper::Message* _msg);
    static std::list<Object*> unpackObjectListFromNetwork(netwrapper::Message* _msg);

    /*!
     * \brief The application must implement this method in order to update the optimistic
     *        state of the object. The library will always update the optimistic state of
     *        an object prior to make the optimistic delivery of a given command when the
     *        previous command was possibly not optimistically delivered - or, more generally,
     *        when the previous state is different on the server -, fixing the gap in the
     *        stream of commands.
     * \param _state GameObject containing the up-to-date state.
     */
    virtual void handleNewOptimisticState(GameObject* _state) = 0;

    /*!
     * \brief The application must implement this method in order to update the conservative
     *        state of the object. The library will always update the conservative state of
     *        an object prior to make the conservative delivery of a given command when the
     *        previous command was not conservatively delivered - fixing the gap in the
     *        command stream.
     * \param _state GameObject containing the up-to-date state.
     */
    virtual void handleNewConservativeState(GameObject* _state) = 0;

    /*!
     * \brief The application must implement this method in order to process a command once
     *        it is optimistically received. The optimistic delivery has a cost: although
     *        it reduces the delay from the sending of a command from a player to the
     *        delivering of that command to another player, the order is not guaranteed to
     *        be correct, and the application must somehow deal with the discrepancies
     *        found between the optimistic delivery (faster, but unreliable) and the conservative
     *        delivery (slow, but guaranteed to be obey the correct, final order).
     * \param _cmd The command to be delivered and processed by the application.
     */
    virtual void handleOptimisticDelivery(Message* _cmd) = 0;

    /*!
     * \brief The application must implement this method in order to process a command once
     *        it is optimistically received. The optimistic delivery has a cost: although
     *        it reduces the delay from the sending of a command from a player to the
     *        delivering of that command to another player, the order is not guaranteed to
     *        be correct, and the application must somehow deal with the discrepancies
     *        found between the optimistic delivery (faster, but unreliable) and the conservative
     *        delivery (slow, but guaranteed to be obey the correct, final order).
     * \param _cmd The command to be delivered and processed by the application.
     */
    virtual void handleConservativeDelivery(Message* _cmd) = 0;

  protected:
    int id;
};

}

#endif /* COREOBJECT_H_ */