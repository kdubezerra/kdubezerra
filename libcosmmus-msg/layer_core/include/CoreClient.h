/*
 * CoreClient.h
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef CORECLIENT_H_
#define CORECLIENT_H_

#include <iostream>
#include <string>
#include <list>

// API layer Classes
class Command;
class GameClient;

// core layer Classes
class ServerMessage;

/*!
 * \class CoreClient
 * \brief This class has the main implementation of the client-side of the libcosmmus-msg, except for network layer calls.
 * Instead of invoking network layer primitives directly, the library core just calls abstract network calls from the network layer.
 * The network layer, then, is the one responsible for calling the operating system. This allows for future improvements of the
 * lower lever network implementation, while leaving the core algorithms untouched.
 */
class CoreClient {
  public:
    CoreClient();
    virtual ~CoreClient();
    int connect(std::string _address);
    int disconnect();
    int submitCommand(Command* _cmd);
    int setCallbackClientInterface(GameClient* _cbclient);
    int handleServerMessage(ServerMessage* _serverMsg);
    GameClient* getCallbackClientInterface();

  private:
    int handleCommand(Command* _cmd);
    int handleStateUpdate(Object* _state);
    GameClient* _cbclient;
};

#endif /* CORECLIENT_H_ */
