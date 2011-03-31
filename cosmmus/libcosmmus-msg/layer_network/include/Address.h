/*
 * Address.h
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <string>
#include <SDL/SDL_net.h>

#include "Message.h"

/*
 *
 */
namespace netwrapper {

class Message;

class Address {
  public:
    Address();
    Address(std::string _address, unsigned short _port);
    Address(Address* other);
    virtual ~Address();
    void setAddress (IPaddress _address);
    IPaddress getAddress();
    static Message* pack(Address* _addr);
    static Address* unpack(Message* _msg);
  private:
    IPaddress address;
};

}

#endif /* ADDRESS_H_ */
