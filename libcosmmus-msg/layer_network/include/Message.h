/*
 * Message.h
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <list>
#include <string>
#include <vector>

namespace netwrapper {

class Message {
  public:
    Message();
    Message(Message* _other);
    virtual ~Message();
    bool equals(Message* _other);

    void addBool(bool _bvalue);
    void addChar(char _cvalue);
    void addInt(int _ivalue);
    void addFloat(float _fvalue);
    void addString(const std::string& _svalue);
    void addMessage(Message* _msg);
    void addMessageCopy(Message* _msg);
    void addArbitrary(int length, void* data);

    bool getBool(int _pos);
    int getBoolCount();

    char getChar(int _pos);
    int getCharCount();

    int getInt(int _pos);
    int getIntCount();

    float getFloat(int _pos);
    int getFloatCount();

    const std::string getString(int _pos);
    int getStringCount();

    Message* getMessage(int _pos);
    int getMessageCount();

    const void* getArbitrary();
    int getArbitraryLength();

    char* getSerializedMessage();
    int getSerializedLength();
    void buildFromBuffer(char* _buffer);

  private:
    std::vector<bool> boolList;
    std::vector<char> charList;
    std::vector<int> intList;
    std::vector<float> floatList;
    std::vector<std::string> stringList;
    std::vector<Message*> messageList;
    char* arbitraryData;
    int arbitraryLength;
};

}

#endif /* MESSAGE_H_ */
