#pragma once

#include <set>
#include "RemoteAbstract.h"
#include "ServerEventInterface.h"

namespace P2PSEL0
{
  class CRemoteClient: public CRemoteAbstract
  {
  public:
    CRemoteClient():CRemoteAbstract(), m_pReference(NULL), m_poInterface(NULL) {}
    /// Constructor
    /// Não responsável pela construção e destruição de stations
    CRemoteClient(station_c& oStation, TClientID id): CRemoteAbstract(oStation, id), m_pReference(NULL), m_dLastPacketReceiveTime(get_time()), m_poInterface(NULL) {}
    /// Destructor
    ~CRemoteClient() {}
    /// Operator =
    CRemoteClient& operator=(const CRemoteClient& oRemote);
    /// Group with list
    template< class T >
      void SetWishGroup(T* pGroup)
      {
        m_pReference = pGroup;
      }
    /// Do I have a wish?
    const bool HasWish() const {return m_pReference != NULL;}
    /// Get my personal wish group
    template< class T >
      T* GetWishGroup(const bool bClear)
      {
        T* pTemp = (T*)m_pReference;
        if (bClear)
          m_pReference = NULL;
        return pTemp;
      }
    /// Timeout?
    //const bool Timeout(); //NOT IMPLEMENTED!
    /// Clear connections
    void ClearConnections() { oConnectedClient.clear(); }
    /// Check if is connected to me
    const bool IsConnected(TClientID id);
    /// Timeout
    const bool Timeout();
    /// Set server callback interface
    void SetCallbackInterface(CServerEventInterface& oServerEventInterface) { m_poInterface = &oServerEventInterface; }
    /// Group timeout when attempting to enter in a group
    const bool GroupTimeout();
    /// Restart group timeout
    void RestartGroupTimeout() { m_dGroupJoinAttemptTime = get_time(); }

  protected:
    virtual void station_incoming_data(buffer_c& pkt, int packet_id);

    virtual void station_connected(buffer_c& connection_pkt, const bool forced);

    virtual void station_connection_timed_out();

    virtual void station_connection_refused(buffer_c& reject_pkt);

    virtual void station_disconnected(bool remote, buffer_c& reason);

    /// Reference to group that I want to enter
    void* m_pReference;
    /// Last packed received time
    double m_dLastPacketReceiveTime;
    /// Connection set
    std::set< TClientID > oConnectedClient;
    /// Callback interface
    CServerEventInterface* m_poInterface;
    /// Group timeout value
    double m_dGroupJoinAttemptTime;
  };
}
