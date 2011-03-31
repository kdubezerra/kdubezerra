/*
 * PeerInterface.h
 *
 *  Created on: 10/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef PEERINTERFACE_H_
#define PEERINTERFACE_H_

/*
 *
 */
namespace netwrapper {

class Message;

class PeerInterface {
  public:
    PeerInterface();
    virtual ~PeerInterface();

    virtual void handlePeerMessage(Message* _msg) = 0;
};

}

#endif /* PEERINTERFACE_H_ */
