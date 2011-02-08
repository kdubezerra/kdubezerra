/*
 * GameObject.h
 *
 *  Created on: 31/01/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <list>
#include <map>

namespace cosmmusmsg {

class GameCommand;
class Message;
class Player;

class GameObject {
  public:
    GameObject();
    virtual ~GameObject();

    void setId(long _id);
    long getId();

    static std::map<long,GameObject*> getObjectList();
    static GameObject* getObjectById(long _id);

    void subscribe (Player* _player);
    void unsubscribe (Player* _player);
    std::list<Player*> getSubscribers();

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

    /*!
    * \brief As the library needs to instantiate application objects, whose type is not
    *        known by it, it must use a callback function, delegating the task of instantiating
    *        objects to the application developer.
    * \return It returns a pointer to the newly instantiated application's extended object.
    */
    virtual GameObject* createNew() = 0;

  private:
    long id;
    std::list<Player*> subscriberList;
    static std::map<long, GameObject*> objectList;
};

}

#endif /* GAMEOBJECT_H_ */
