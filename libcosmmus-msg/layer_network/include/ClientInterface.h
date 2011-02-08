/*
 * ClientInterface.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef CLIENTINTERFACE_H_
#define CLIENTINTERFACE_H_

/*
 *
 */
namespace netwrapper {

class Message;

class ClientInterface {
  public:
    ClientInterface();
    virtual ~ClientInterface();

    virtual void handleMessage(Message* _msg) = 0;
};

}

#endif /* CLIENTINTERFACE_H_ */
