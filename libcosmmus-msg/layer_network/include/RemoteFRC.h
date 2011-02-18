/*
 * RemoteFRC.h
 *
 *  Created on: 17/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef REMOTEFRC_H_
#define REMOTEFRC_H_

#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

namespace netwrapper {

class RemoteFRC {
  public:
    RemoteFRC();
    virtual ~RemoteFRC();
    TCPsocket getSocket() const;
    void setSocket(TCPsocket _clientSocket);
  private:
    TCPsocket clientSocket;
};

}



#endif /* REMOTEFRC_H_ */
