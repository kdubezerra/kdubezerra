/*
 * Message.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include "../include/Message.h"

using namespace netwrapper;

Message::Message() {
  arbitraryData = NULL;
  arbitraryLength = 0;
}

Message::Message(Message* _msg) {
  floatList = _msg->floatList;
  intList = _msg->intList;
  stringList = _msg->stringList;
  arbitraryLength = _msg->arbitraryLength;
  arbitraryData = new void*[arbitraryLength];
  memcpy(arbitraryData, _msg->arbitraryData, arbitraryLength);
  for (std::list<Message*>::iterator it = _msg->messageList.begin() ; it != _msg->messageList.end() ; it++) addMessage(*it);
}

Message::~Message() {
  for (std::vector<Message*>::iterator it = messageList.begin() ; it != messageList.end() ; it++)
    delete *it;
  if (arbitraryData != NULL)
    delete arbitraryData;
}

int Message::addInt(int _ivalue) {
  intList.push_back(_ivalue);
}

int Message::addFloat(float _fvalue) {
  floatList.push_back(_fvalue);
}

int Message::addString(const std::string& _svalue) {
  stringList.push_back(newString);
}

int Message::addMessage(Message* _msg) {
  messageList.push_back(_msg);
}

int Message::addMessageCopy(Message* _msg) {
  Message* newMessage = new Message(_msg);
  messageList.push_back(newMessage);
}

int Message::getInt(int _pos) {
  return intList[_pos];
}

long Message::getIntCount() {
  return intList.size();
}

int Message::getFloat(int _pos) {
  return floatList[_pos];
}

long Message::getFloatCount() {
  return floatList.size();
}

const std::string Message::getString(int _pos) {
  return stringList[_pos];
}

long Message::getStringCount() {
  return stringList.size();
}
