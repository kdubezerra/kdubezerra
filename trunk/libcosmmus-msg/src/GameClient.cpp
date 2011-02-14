/*
 * GameClient.cpp
 *
 *  Created on: 03/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/GameClient.h"
#include "../layer_core/include/Client.h"

using namespace cosmmusmsg;
using namespace optpaxos;
using namespace netwrapper;

GameClient::GameClient() {
  objectModel = NULL;
  Client* coreClient = new Client();
  coreClient->setCallbackInterface(this);
}

GameClient::~GameClient() {
  delete coreClient;
}

/*!
 * \brief Connects the game client to a given game server. If already connected to a different a server, disconnect first.
 * \param _address The address of the game server, may be in the forms of url, ip:port or url:port.
 * \return The result of the connection: 0 if successful; not 0 otherwise.
 */
int GameClient::connect(std::string _address, unsigned _port) {
  return coreClient->connect(_address, _port);
}

/*!
 * \brief Disconnects from the server.
 * \return The result of the disconnection: 0 if it was a "clean" disconnection.
 */
int GameClient::disconnect() {
  return coreClient->disconnect();
}

/*!
 * \brief Send a command to the server, which commits it to the overall game server system.
 * \param _cmd The GameCommand object, which contains the details of the command (application-defined type of command and its targets, if any).
 * \return The result of the submission: 0 if submitted successfully to the server.
  */
void GameClient::submitCommand(Command* _cmd) {
  coreClient->submitCommand(_cmd);
}

/*!
 * \brief Submit some application-defined request to the server.
 * \param _request The request message object.
 * \return The result of the submission: 0 if submitted successfully to the server.
 */
int GameClient::submitRequest(Message* _request) {
  return 0;
}

void GameClient::setObjectModel(GameObject* _objModel) {
  objectModel = _objModel;
}

std::list<GameObject*> GameClient::getObjectList() {
  return objList;
}
