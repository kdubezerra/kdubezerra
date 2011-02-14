/*
 * OPMessage.h
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OPMESSAGE_H_
#define OPMESSAGE_H_

#include <list>
#include "../../layer_network/include/Message.h"

namespace optpaxos {

enum OPMessageType { APP_MSG, CLIENT_CMD, CMD_TO_COORD, CMD_OPT, ACCEPT_MSG, ACCEPTED_MSG, LEARN_MSG, CMD_DELIVERY };

class Command;
class Object;

/*!
 * \class OPMessage
 * \brief This class may contain: a list of commands, updates and/or an extra payload which was produced in the application level
 */
class OPMessage {
  public:
    OPMessage();
    OPMessage(OPMessage* _other);
    virtual ~OPMessage();

    void setType(OPMessageType _msgType);
    void addCommand(Command* _cmd);
    void setCommandList(std::list<Command*> _cmdList);
    void addState(Object* _state);
    void setStateList(std::list<Object*> _stateList);
    void setExtraPayload(netwrapper::Message* _payload);

    OPMessageType getType();
    std::list<Command*> getCommandList();
    std::list<Object*> getStateList();
    netwrapper::Message* getExtraPayload();

    bool hasCommand();
    bool hasState();
    bool hasExtraPayload();

    static netwrapper::Message* packToNetwork(OPMessage* _opMsg);
    static OPMessage* unpackFromNetwork(netwrapper::Message* _msg);

  private:
    OPMessageType messageType;
    std::list<Command*> commandList;
    std::list<Object*> stateList;
    netwrapper::Message* extraPayload;
};

}

#endif /* OPMESSAGE_H_ */
