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
  arbitraryData = new char[arbitraryLength];
  memcpy(arbitraryData, _msg->arbitraryData, arbitraryLength);
  for (std::vector<Message*>::iterator it = _msg->messageList.begin() ; it != _msg->messageList.end() ; it++) addMessage(new Message(*it));
}

Message::~Message() {
  for (std::vector<Message*>::iterator it = messageList.begin() ; it != messageList.end() ; it++)
    delete *it;
  if (arbitraryData != NULL)
    delete [] arbitraryData;
}

bool Message::equals(Message* _other) {
  if (this->intList != _other->intList
    || this->floatList != _other->floatList
    || this->stringList != _other->stringList
    || this->arbitraryLength != _other->arbitraryLength)
    return false;

  std::vector<Message*>::iterator itthis = this->messageList.begin();
  std::vector<Message*>::iterator itother = _other->messageList.begin();
  while(true) {
    if (itthis == this->messageList.end() || itother == _other->messageList.end()) {
      if (itthis == this->messageList.end() && itother == _other->messageList.end())
        break;
      else
        return false;
    }
    if ( !(*itthis)->equals(*itother) )
      return false;
    itthis++;
    itother++;
  }

  if (memcmp(this->arbitraryData, _other->arbitraryData, this->arbitraryLength) != 0)
    return false;

  return true;
}

int Message::addInt(int _ivalue) {
  intList.push_back(_ivalue);
}

int Message::addFloat(float _fvalue) {
  floatList.push_back(_fvalue);
}

int Message::addString(const std::string& _svalue) {
  stringList.push_back(_svalue);
}

int Message::addMessage(Message* _msg) {
  messageList.push_back(_msg);
}

int Message::addMessageCopy(Message* _msg) {
  messageList.push_back(new Message(_msg));
}

int Message::getInt(int _pos) {
  return intList[_pos];
}

int Message::getIntCount() {
  return intList.size();
}

float Message::getFloat(int _pos) {
  return floatList[_pos];
}

int Message::getFloatCount() {
  return floatList.size();
}

const std::string Message::getString(int _pos) {
  return stringList[_pos];
}

int Message::getStringCount() {
  return stringList.size();
}
