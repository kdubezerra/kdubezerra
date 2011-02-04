/*
 * Message.h
 *
 *  Created on: 04/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <vector>
#include <string>

/*
 *
 */
class Message {
  public:
    Message();
    virtual ~Message();
    int addInt(int _ivalue);
    int addFloat(float _fvalue);
    int addString(std::string& _svalue);

    int getInt(int _pos);
    long getIntCount();

    int getFloat(int _pos);
    long getFloatCount();

    const std::string getString(int _pos);
    long getStringCount();

  private:
    std::vector<int> intList;
    std::vector<float> floatList;
    std::vector<std::string> stringList;
};

#endif /* MESSAGE_H_ */
