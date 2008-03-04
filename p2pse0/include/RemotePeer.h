#pragma once

#include "RemoteAbstract.h"
#include "ClientEventInterface.h"

namespace P2PSEL0
{
  class CRemotePeer: public CRemoteAbstract
  {
  public:
    CRemotePeer():CRemoteAbstract(), m_poInterface(NULL), m_bInformServer(false) {}
    /// Constructor
    /// Não responsável pela construção e destruição de stations
    CRemotePeer(station_c& oStation, TClientID id): CRemoteAbstract(oStation, id), m_poInterface(NULL) {}
    /// Destructor
    ~CRemotePeer() {}
    /// Operator =
    CRemotePeer& operator=(const CRemotePeer& oRemote);
    /// Set server callback interface
    void SetCallbackInterface(CClientEventInterface& oClientEventInterface) { m_poInterface = &oClientEventInterface; }

    const bool GetInformServerStatus() { return m_bInformServer; }

    void ResetInformServerStatus() { m_bInformServer = false; }

  protected:
    virtual void station_incoming_data(buffer_c& pkt, int packet_id);

    virtual void station_connected(buffer_c& connection_pkt, const bool forced);

    virtual void station_connection_timed_out();

    virtual void station_connection_refused(buffer_c& reject_pkt);

    virtual void station_disconnected(bool remote, buffer_c& reason);

    /// Callback interface
    CClientEventInterface* m_poInterface;

    bool m_bInformServer;
  };
}
