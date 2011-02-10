/*
 * Client.h
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <iostream>
#include <string>
#include <list>

#include "../../layer_network/include/ClientInterface.h"
#include "../../layer_network/include/FIFOReliableClient.h"
#include "../../layer_network/include/Message.h"

namespace optpaxos {

// core layer Classes
class ClientInterface;
class Command;
class Object;
class OPMessage;

/*!
 * \class Client
 * \brief This class has the main implementation of the client-side of the libcosmmus-msg, except for network layer calls.
 * Instead of invoking network layer primitives directly, the library core just calls abstract network calls from the network layer.
 * The network layer, then, is the one responsible for calling the operating system. This allows for future improvements of the
 * lower lever network implementation, while leaving the core algorithms untouched.
 */
class Client : public netwrapper::ClientInterface,
               private OPTPaxosControl {
  public:
    Client();
    virtual ~Client();
    int connect(std::string _address);
    int disconnect();
    void submitCommand(Command* _cmd);
    void submitApplicationMessage(netwrapper::Message* _msg);
    void setCallbackInterface(optpaxos::ClientInterface* _callbackClient);
    optpaxos::ClientInterface* getCallbackClient();
    void handleServerMessage(netwrapper::Message* _msg);

  private:
    void handleOPMessage(OPMessage* _serverMsg);
    void handleCommand(Command* _cmd);
    void handleStateUpdate(Object* _state);

    optpaxos::ClientInterface* callbackClient;
    netwrapper::FIFOReliableClient* netClient;
};

}

#endif /* CLIENT_H_ */
