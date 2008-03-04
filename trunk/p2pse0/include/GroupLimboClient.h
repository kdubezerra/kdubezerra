#pragma once

#include "GroupAbstract.h"
#include "RemotePeer.h"

namespace P2PSEL0
{
  class CGroupLimboClient: public CGroupAbstract< CRemotePeer >
  {
  public:
    /// Default constructor
    CGroupLimboClient(): CGroupAbstract<CRemotePeer>::CGroupAbstract() {}
    /// Constructor
    CGroupLimboClient(TGroupID id, station_c& oServer, CClientEventInterface& oClientEventInteface): CGroupAbstract<CRemotePeer>::CGroupAbstract(id, 0, oServer), m_poClientEventInterface(&oClientEventInteface) {};
    /// Override server station
    void OverrideStation(station_c& oStation) { m_oServer = oStation; }

    /// Chat message
    virtual const bool Chat(const string& oMessage) { return false; }
    /// Send group
    virtual const bool SendMessage(buffer_c& oData, const int iStream, const bool bReliable = true) { return false; }

  private:
    /// Callback Interface
    CClientEventInterface* m_poClientEventInterface;

    /// List of clients that want to get out of here AFTER SERVER COMMIT (Only limbo to group)
    /// Group to limbo is instantaneous
    std::map< TClientID, TGroupID > m_oPendingClientList;
  };
}
