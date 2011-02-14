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

/*
 *
 */
class Message {
  public:
    Message();
    Message(Message* _other);
    virtual ~Message();
    bool equals(Message* _other);

    int addBool(bool _bvalue);
    int addChar(char _cvalue);
    int addInt(int _ivalue);
    int addFloat(float _fvalue);
    int addString(const std::string& _svalue);
    int addMessage(Message* _msg);
    int addMessageCopy(Message* _msg);
    int addArbitrary(int length, void* data);

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

  private:
    std::vector<int> intList;
    std::vector<float> floatList;
    std::vector<std::string> stringList;
    std::vector<Message*> messageList;
    char* arbitraryData;
    int arbitraryLength;
};

}

#endif /* MESSAGE_H_ */
