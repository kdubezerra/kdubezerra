/*
 * Address.h
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <SDL/SDL_net.h>

/*
 *
 */
namespace netwrapper {

class Address {
  public:
    Address();
    virtual ~Address();
    void setAddress (IPaddress address);
    IPaddress getAddress();
  private:
    IPaddress address;
};

}

#endif /* ADDRESS_H_ */
