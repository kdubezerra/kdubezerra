/*
 * UnreliablePeer.h
 *
 *  Created on: 08/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef UNRELIABLEPEER_H_
#define UNRELIABLEPEER_H_

/*
 *
 */
namespace netwrapper {

class UnreliablePeer {
  public:
    UnreliablePeer();
    virtual ~UnreliablePeer();

    void setCallbackInterface(PeerInterface* _callbackPeer);

  private:
    PeerInterface* callbackPeer;
};

}

#endif /* UNRELIABLEPEER_H_ */
