/*
 * Message.cpp
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

//#include <iostream>
#include <SDL/SDL_net.h>

#include "../include/Message.h"

using namespace netwrapper;

Message::Message() {
  arbitraryData = NULL;
  arbitraryLength = 0;
}

Message::Message(Message* _msg) {
  boolList = _msg->boolList;
  charList = _msg->charList;
  intList = _msg->intList;
  floatList = _msg->floatList;
  stringList = _msg->stringList;
  arbitraryLength = _msg->arbitraryLength;

  if (arbitraryLength > 0) {
    arbitraryData = new char[arbitraryLength];
    memcpy(arbitraryData, _msg->arbitraryData, arbitraryLength);
  }
  else arbitraryData = NULL;

  for (std::vector<Message*>::iterator it = _msg->messageList.begin() ; it != _msg->messageList.end() ; it++)
    addMessage(new Message(*it));
}

Message::Message(char* _buffer) {
  this->buildFromBuffer(_buffer);
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

void Message::addBool(bool _bvalue) {
  boolList.push_back(_bvalue);
}

void Message::addChar(char _cvalue) {
  charList.push_back(_cvalue);
}

void Message::addInt(int _ivalue) {
  intList.push_back(_ivalue);
}

void Message::addFloat(float _fvalue) {
  floatList.push_back(_fvalue);
}

void Message::addLong(long _lvalue) {
  longList.push_back(_lvalue);
}

void Message::addString(const std::string& _svalue) {
  stringList.push_back(_svalue);
}

void Message::addMessage(Message* _msg) {
  messageList.push_back(_msg);
}

void Message::addMessageCopy(Message* _msg) {
  messageList.push_back(new Message(_msg));
}

void Message::setArbitraryData(int _length, void* _data) {
  arbitraryData = new char[_length];
  arbitraryLength = _length;
  memcpy(arbitraryData, _data, _length);
}

bool Message::getBool(int _pos) {
  return boolList[_pos];
}

char Message::getChar(int _pos) {
  return charList[_pos];
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

long Message::getLong(int _pos) {
  return longList[_pos];
}

int Message::getLongCount() {
  return (int) longList.size();

}

const std::string Message::getString(int _pos) {
  return stringList[_pos];
}

int Message::getStringCount() {
  return stringList.size();
}

Message* Message::getMessage(int _pos) {
  return messageList[_pos];

}

char* Message::getSerializedMessage() {
  int serializedLength = getSerializedLength();
  char* buffer = new char[serializedLength];
  char* bufferpos = buffer;
  SDLNet_Write32(serializedLength, bufferpos);
  bufferpos += 4;
  // *******************************************************\\ BOOL LIST
  SDLNet_Write32((Uint32) boolList.size(), bufferpos);
  bufferpos += 4;
  for (std::vector<bool>::iterator it = boolList.begin() ; it != boolList.end() ; it++) {
    *((bool*)bufferpos) = *it;
    bufferpos++;
  }
  // *******************************************************\\ CHAR LIST
  SDLNet_Write32((Uint32) charList.size(), bufferpos);
  bufferpos += 4;
  for (std::vector<char>::iterator it = charList.begin() ; it != charList.end() ; it++) {
    *bufferpos = *it;
    bufferpos++;
  }
  // *******************************************************\\ INT LIST
  SDLNet_Write32((Uint32) intList.size(), bufferpos);
  bufferpos += 4;
  for (std::vector<int>::iterator it = intList.begin() ; it != intList.end() ; it++) {
    SDLNet_Write32((Uint32) *it, bufferpos);
    bufferpos += 4;
  }
  // *******************************************************\\ FLOAT LIST
  SDLNet_Write32((Uint32) floatList.size(), bufferpos);
  bufferpos += 4;
  for (std::vector<float>::iterator it = floatList.begin() ; it != floatList.end() ; it++) {
    Uint32 send32 = *(reinterpret_cast<Uint32*>(&(*it)));
    SDLNet_Write32(send32, bufferpos);
    bufferpos += 4;
  }
  // *******************************************************\\ LONG LIST
  SDLNet_Write32((Uint32) longList.size(), bufferpos);
  bufferpos += 4;
  for (std::vector<long>::iterator it = longList.begin() ; it != longList.end() ; it++) {
    long longData = *it;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      int width = LONG_BIT;
      int i = 0;
      while (width > 0) {
        bufferpos[i++] = (longData >> (width = width - 8)) & 0xFF;
      }
    #else
      int width = LONG_BIT;
      int i = (LONG_BIT / 8) - 1;
      while (width > 0) {
        bufferpos[i--] = (longData >> (width = width - 8)) & 0xFF;
      }
    #endif
    bufferpos += 8;
  }
  // *******************************************************\\ STRING LIST
  SDLNet_Write32((Uint32) stringList.size(), bufferpos);
  bufferpos += 4;
  for (std::vector<std::string>::iterator it = stringList.begin() ; it != stringList.end() ; it++) {
    SDLNet_Write32(1 + (Uint32) it->size(), bufferpos);
    bufferpos += 4;
    strcpy(bufferpos, it->c_str());
    bufferpos += 1 + (int) it->size();
  }
  // *******************************************************\\ MESSAGE LIST
  SDLNet_Write32((Uint32) messageList.size(), bufferpos);
  bufferpos += 4;
  for (std::vector<Message*>::iterator it = messageList.begin() ; it != messageList.end() ; it++) {
    char* serializedMsg = (*it)->getSerializedMessage();
    Uint32 msgLength = (Uint32) (*it)->getSerializedLength();
    memcpy(bufferpos, serializedMsg, msgLength);
    delete [] serializedMsg;
    bufferpos += msgLength;
  }
  // *******************************************************\\ ARBITRARY DATA
  SDLNet_Write32((Uint32) arbitraryLength, bufferpos);
  bufferpos += 4;
  if (arbitraryLength > 0)
    memcpy(bufferpos, arbitraryData, arbitraryLength);

  return buffer;
}

int Message::getSerializedLength() {
  int length = 4;
  length += 4 + (int) boolList.size();
  length += 4 + (int) charList.size();
  length += 4 + 4 * (int) intList.size();
  length += 4 + 4 * (int) floatList.size();
  length += 4 + 8 * (int) longList.size();
  length += 4;
  for (std::vector<std::string>::iterator it = stringList.begin() ; it != stringList.end() ; it++) {
    length += 4;
    length += (int) it->size();
    length++;
  }
  length += 4 + arbitraryLength;
  length += 4;
  for (std::vector<Message*>::iterator it = messageList.begin() ; it != messageList.end() ; it++) {
    length += (*it)->getSerializedLength();
  }
  return length;
}

void Message::buildFromBuffer(char* _buffer) {
  char* bufferpos = _buffer + 4;
  int boolCount = SDLNet_Read32(bufferpos);
  bufferpos += 4;
  for (int i = 0 ; i < boolCount ; i++) {
    this->addBool(*((bool*) bufferpos));
    bufferpos++;
  }
  int charCount = SDLNet_Read32(bufferpos);
  bufferpos += 4;
  for (int i = 0 ; i < charCount ; i++) {
    this->addChar(*bufferpos);
    bufferpos++;
  }
  int intCount = SDLNet_Read32(bufferpos);
  bufferpos += 4;
  for (int i = 0 ; i < intCount ; i++) {
    this->addInt(SDLNet_Read32(bufferpos));
    bufferpos += 4;
  }
  int floatCount = SDLNet_Read32(bufferpos);
  bufferpos += 4;
  for (int i = 0 ; i < floatCount ; i++) {
    Uint32 recvd32 = SDLNet_Read32(bufferpos);
    this->addFloat(*(reinterpret_cast<float*>(&recvd32)));
    bufferpos += 4;
  }
  int longCount = SDLNet_Read32(bufferpos);
  bufferpos += 4;
  for (int i = 0 ; i < longCount ; i++) {
    long newLong = 0;
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
      int width = LONG_BIT;
      int i = 0;
      while (width > 0) {
        newLong = newLong | (((Uint8 *)bufferpos)[i++] << (width = width - 8));
      }
    #else
      int width = LONG_BIT;
      int i = (LONG_BIT / 8) - 1;
      while (width > 0) {
        newLong = newLong | (((Uint8 *)bufferpos)[i--] << (width = width - 8));
      }
    #endif
    longList.push_back(newLong);
    bufferpos += 8;
  }
  int stringCount = SDLNet_Read32(bufferpos);
  bufferpos += 4;
  for (int i = 0 ; i < stringCount ; i++) {
    int strLength = SDLNet_Read32(bufferpos);
    bufferpos += 4;
    this->addString(bufferpos);
    bufferpos += strLength;
  }
  int messageCount = SDLNet_Read32(bufferpos);
  bufferpos += 4;
  for (int i = 0 ; i < messageCount ; i++) {
    Message* msg = new Message();
    // The length field is actually part of the msg, so the bufferpos stays the same
    int msgLength = SDLNet_Read32(bufferpos);
    msg->buildFromBuffer(bufferpos);
    this->addMessage(msg);
    bufferpos += msgLength;
  }
  this->arbitraryLength = SDLNet_Read32(bufferpos);
  bufferpos += 4;
  if (arbitraryLength != 0) {
    arbitraryData = new char[arbitraryLength];
    memcpy(arbitraryData, bufferpos, arbitraryLength);
  }
  else arbitraryData = NULL;
}
