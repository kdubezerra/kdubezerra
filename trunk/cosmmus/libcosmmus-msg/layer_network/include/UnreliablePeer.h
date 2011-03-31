/*
 * UnreliablePeer.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef UNRELIABLEPEER_H_
#define UNRELIABLEPEER_H_

#include <list>

#include "GenericNode.h"
#include "PeerInterface.h"

/*
 *
 */
namespace netwrapper {

class UnreliablePeer {
  public:
    UnreliablePeer();
    virtual ~UnreliablePeer();

    int init(unsigned _port = 0);

    int sendMessage(Message* _msg, Address* _address);
    int checkNewMessages();

    void setCallbackInterface(PeerInterface* _callbackPeer);
    netwrapper::PeerInterface* getCallbackInterface();

  private:
    PeerInterface* callbackPeer;
    UDPsocket peerSocket;

    /** MUST BE DELETED! **/std::list<Message*> messageList;
};

}

#endif /* UNRELIABLEPEER_H_ */
