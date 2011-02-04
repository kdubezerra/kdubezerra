/*
 * ServerMessage.h
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef SERVERMESSAGE_H_
#define SERVERMESSAGE_H_

#include <list>

// API layer classes
class Message;
class Command;
class Object;

/*!
 * \class ServerMessage
 * \brief This class may contain: a list of commands and/or an extra payload which was produced in the application level
 */
class ServerMessage {
  public:
    ServerMessage();
    virtual ~ServerMessage();

    int getType();
    std::list<Command*> getCommandList();
    std::list<Object*> getStateList();
    Message* getExtraPayload();

    bool hasCommand();
    bool hasState();
    bool hasExtraPayload();

  private:
    int messageType;
    std::list<Command*> commandList;
    std::list<Object*> stateList;
    Message* extraPayload;
};

#endif /* SERVERMESSAGE_H_ */
