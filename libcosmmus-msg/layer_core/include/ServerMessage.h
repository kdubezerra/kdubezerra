/*
 * ServerMessage.h
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef SERVERMESSAGE_H_
#define SERVERMESSAGE_H_

#include <list>
#include "../../layer_network/include/Message.h"

namespace optpaxos {

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

    void setType(int _msgType);
    void addCommand(Command* _cmd);
    void setCommandList(std::list<Command*> _cmdList);
    void addState(Object* _state);
    void setStateList(std::list<Object*> _objList);
    void addExtraPayload(netwrapper::Message* _payload);

    int getType();
    std::list<Command*> getCommandList();
    std::list<Object*> getStateList();
    netwrapper::Message* getExtraPayload();

    bool hasCommand();
    bool hasState();
    bool hasExtraPayload();

  private:
    int messageType;
    std::list<Command*> commandList;
    std::list<Object*> stateList;
    netwrapper::Message* extraPayload;
};

}

#endif /* SERVERMESSAGE_H_ */
