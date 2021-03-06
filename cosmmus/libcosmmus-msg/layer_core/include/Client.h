/*
 * Client.h
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */
/// TODO: leave as public _only_ the methods which should be visible to the application. Leave all else as private and,
///       when necessary, use the _friend_ modifier...
///       ...
///       or use the Proxy design pattern.

#ifndef CLIENT_H_
#define CLIENT_H_

#include <iostream>
#include <string>
#include <list>

#include "../../layer_network/include/ClientInterface.h"
#include "../../layer_network/include/FIFOReliableClient.h"
#include "../../layer_network/include/Message.h"

namespace optpaxos {

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
class Client : public netwrapper::ClientInterface {
  public:
    Client();
    virtual ~Client();
    int connect(std::string _address, unsigned port);
    void disconnect();
    void submitCommand(Command* _cmd);
    void submitRequest(netwrapper::Message* _msg);
    void handleServerMessage(netwrapper::Message* _msg);
    virtual void handleMessage(netwrapper::Message* _msg) = 0;

    int checkAll();
    int checkNewMessages();

  private:
    void handleCommand(Command* _cmd);
    void handleStateUpdate(Object* _state);

    netwrapper::FIFOReliableClient* netClient;
};

}

#endif /* CLIENT_H_ */
