/*
 * UnreliablePeer.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef UNRELIABLEPEER_H_
#define UNRELIABLEPEER_H_

#include "GenericNode.h"
#include "PeerInterface.h"

/*
 *
 */
namespace netwrapper {

class UnreliablePeer : public GenericNode {
  public:
    UnreliablePeer();
    virtual ~UnreliablePeer();

    int init(unsigned _port);

    void setCallbackInterface(PeerInterface* _callbackPeer);

    netwrapper::PeerInterface* getCallbackPeer();
    int sendMessage(Message* _msg, Address* _address);
  private:
    PeerInterface* callbackPeer;
};

}

#endif /* UNRELIABLEPEER_H_ */
